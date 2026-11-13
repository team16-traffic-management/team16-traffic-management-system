#!/usr/bin/env python3
"""Local browser dashboard backed by the existing C++17 traffic simulator."""
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json, re, subprocess, tempfile

ROOT=Path(__file__).resolve().parents[1]
WEB=Path(__file__).resolve().parent
BINARY=ROOT/"build"/"traffic-management"
SAFE=re.compile(r"^[A-Za-z0-9 _-]{1,32}$")

def name(value,label):
    value=str(value or "").strip()
    if not SAFE.fullmatch(value): raise ValueError(f"{label} must be 1–32 letters, numbers, spaces, _ or -.")
    return value

def build():
    r=subprocess.run(["make","build/traffic-management"],cwd=ROOT,capture_output=True,text=True,timeout=90)
    if r.returncode: raise RuntimeError((r.stderr or r.stdout)[-4000:])

def config(d):
    ticks=int(d.get("ticks",100)); cap=int(d.get("queueCapacity",15))
    arrival=float(d.get("arrivalRate",.4)); emergency=float(d.get("emergencyProbability",.05))
    if not 1<=ticks<=10000: raise ValueError("Ticks must be between 1 and 10,000.")
    if not 1<=cap<=10000: raise ValueError("Queue capacity must be between 1 and 10,000.")
    if not 0<=arrival<=1 or not 0<=emergency<=1: raise ValueError("Probabilities must be between 0 and 1.")
    ints=d.get("intersections") or []
    if not 1<=len(ints)<=8: raise ValueError("Add between 1 and 8 intersections.")
    lines=[f"total_ticks={ticks}",f"arrival_rate={arrival}",f"emergency_probability={emergency}",f"adaptive_mode={'true' if d.get('adaptiveMode') else 'false'}",f"queue_capacity={cap}"]
    ids=set(); lane_map={}
    for item in ints:
        ident=name(item.get("id"),"Intersection ID")
        if ident in ids: raise ValueError(f"Duplicate intersection ID: {ident}")
        ids.add(ident); lanes=[name(x,"Lane name") for x in item.get("lanes",[])]
        if not lanes or len(lanes)>12 or len(set(lanes))!=len(lanes): raise ValueError(f"{ident} needs 1–12 unique lanes.")
        lane_map[ident]=lanes; lines.append(f"intersection.{ident}.lanes={','.join(lanes)}")
    for link in d.get("connections",[]):
        src=name(link.get("from"),"Connection source"); dst=name(link.get("to"),"Connection destination")
        if src not in ids or dst not in ids: raise ValueError("Connections must use existing intersections.")
        if src==dst: raise ValueError("An intersection cannot connect to itself.")
        if set(lane_map[src])!=set(lane_map[dst]): raise ValueError(f"{src} and {dst} must have matching lane names.")
        lines.append(f"connection.{src}={dst}")
    return "\n".join(lines)+"\n"

def metrics(out):
    pats={"throughput":r"Total Throughput \(Processed\)\s*:\s*(\d+)","averageWait":r"Overall Average Wait Time\s*:\s*([\d.]+)","blocked":r"Total Overflow \(Blocked\) Vehicles\s*:\s*(\d+)","preemptions":r"Total Emergency Preemption Events\s*:\s*(\d+)","ticksExecuted":r"\((\d+) ticks executed\)","transfers":r"Total successful vehicle transfers:\s*(\d+)","pendingTransfers":r"Vehicles still waiting for transfer:\s*(\d+)"}
    result={}
    for k,p in pats.items():
        m=re.search(p,out); result[k]=(float(m.group(1)) if k=="averageWait" else int(m.group(1))) if m else 0
    rows=[]; active=False
    for line in out.splitlines():
        if "Per-Intersection Lane Breakdown:" in line: active=True; continue
        if active and line.startswith("===="): break
        if active and re.match(r"^\s*[\w -]+\s{2,}[\w -]+\s{2,}\d+",line):
            p=line.split()
            if len(p)>=5 and p[2].isdigit(): rows.append({"intersection":p[0],"lane":p[1],"throughput":int(p[2]),"averageWait":float(p[3]),"maxQueue":int(p[4])})
    result["lanes"]=rows; result["termination"]="tick limit reached"
    return result

class Handler(BaseHTTPRequestHandler):
    def json(self,status,data):
        body=json.dumps(data).encode(); self.send_response(status); self.send_header("Content-Type","application/json; charset=utf-8"); self.send_header("Content-Length",str(len(body))); self.send_header("Cache-Control","no-store"); self.end_headers(); self.wfile.write(body)
    def do_GET(self):
        if self.path=="/api/health": return self.json(200,{"ok":True,"engine":"C++17"})
        files={"/":("index.html","text/html; charset=utf-8"),"/index.html":("index.html","text/html; charset=utf-8"),"/styles.css":("styles.css","text/css; charset=utf-8"),"/app.js":("app.js","text/javascript; charset=utf-8")}
        if self.path not in files: return self.send_error(404)
        file,mime=files[self.path]; body=(WEB/file).read_bytes(); self.send_response(200); self.send_header("Content-Type",mime); self.send_header("Content-Length",str(len(body))); self.end_headers(); self.wfile.write(body)
    def do_POST(self):
        if self.path!="/api/simulate": return self.send_error(404)
        try:
            length=int(self.headers.get("Content-Length","0"))
            if not 1<=length<=20000: return self.json(400,{"error":"Invalid request size."})
            cfg=config(json.loads(self.rfile.read(length)))
            if not BINARY.exists(): build()
            with tempfile.TemporaryDirectory(prefix="tms-") as tmp:
                path=Path(tmp)/"run.cfg"; path.write_text(cfg,encoding="utf-8")
                run=subprocess.run([str(BINARY),str(path)],cwd=ROOT,capture_output=True,text=True,timeout=30)
            output=run.stdout+("\n"+run.stderr if run.stderr else "")
            if run.returncode: return self.json(422,{"error":output[-4000:] or "Simulation failed."})
            return self.json(200,{"metrics":metrics(output),"output":output[-16000:],"config":cfg})
        except ValueError as e: return self.json(400,{"error":str(e)})
        except subprocess.TimeoutExpired: return self.json(408,{"error":"Simulation exceeded the 30-second safety limit."})
        except Exception as e: return self.json(500,{"error":str(e)})
    def log_message(self,fmt,*args): print("[TMS Web] "+fmt%args)

if __name__=="__main__":
    build(); http=ThreadingHTTPServer(("127.0.0.1",8000),Handler)
    print("Traffic dashboard: http://127.0.0.1:8000 (local only)")
    try: http.serve_forever()
    except KeyboardInterrupt: print("\nDashboard stopped.")
    finally: http.server_close()
