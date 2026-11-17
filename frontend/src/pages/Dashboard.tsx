import { useEffect } from 'react'
import {
  AreaChart,
  Area,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from 'recharts'
import {
  Activity,
  AlertTriangle,
  ArrowUpRight,
  Car,
  CheckCircle2,
  Clock,
  Layers,
  Pause,
  Play,
  RotateCcw,
  ShieldAlert,
  Zap,
} from 'lucide-react'
import { useSimStore } from '../store/simStore'
import type { LaneData } from '../api/simulation'

function SignalBadge({ signal }: { signal: 'RED' | 'YELLOW' | 'GREEN' }) {
  const styles = {
    GREEN: 'bg-emerald-500/10 text-emerald-400 border-emerald-500/40 shadow-[0_0_12px_rgba(52,211,153,0.3)]',
    YELLOW: 'bg-amber-500/10 text-amber-400 border-amber-500/40 shadow-[0_0_12px_rgba(251,191,36,0.3)]',
    RED: 'bg-rose-500/10 text-rose-400 border-rose-500/40 shadow-[0_0_12px_rgba(244,63,94,0.3)]',
  }
  const dots = {
    GREEN: 'bg-emerald-400 shadow-[0_0_8px_#34d399]',
    YELLOW: 'bg-amber-400 shadow-[0_0_8px_#fbbf24]',
    RED: 'bg-rose-500 shadow-[0_0_8px_#f43f5e]',
  }
  return (
    <div className={`inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full border text-xs font-mono font-semibold ${styles[signal]}`}>
      <span className={`w-2 h-2 rounded-full ${dots[signal]}`} />
      <span>{signal}</span>
    </div>
  )
}

function LaneRow({ lane }: { lane: LaneData }) {
  const pct = lane.capacity > 0 ? Math.min(100, Math.round((lane.queueSize / lane.capacity) * 100)) : 0
  const barColor =
    pct >= 80 ? 'bg-gradient-to-r from-amber-500 to-rose-500' :
    pct >= 50 ? 'bg-gradient-to-r from-cyan-500 to-amber-500' :
    'bg-gradient-to-r from-blue-500 to-cyan-400'

  return (
    <tr className="border-b border-slate-800/60 hover:bg-slate-800/30 transition-colors">
      <td className="py-3.5 px-4">
        <div className="flex items-center gap-2">
          <div className="w-1.5 h-6 rounded-full bg-cyan-500/50" />
          <div>
            <div className="font-semibold text-slate-100 text-sm">{lane.name.replace('_', ' ')}</div>
            <div className="text-[11px] font-mono text-slate-500 uppercase">{lane.name}</div>
          </div>
        </div>
      </td>
      <td className="py-3.5 px-4">
        <SignalBadge signal={lane.signal} />
      </td>
      <td className="py-3.5 px-4">
        <div className="space-y-1.5 max-w-xs">
          <div className="flex justify-between text-xs font-mono">
            <span className="text-slate-300 font-semibold">{lane.queueSize} <span className="text-slate-500 font-normal">/ {lane.capacity}</span></span>
            <span className={pct > 75 ? 'text-rose-400 font-bold' : 'text-cyan-400'}>{pct}%</span>
          </div>
          <div className="w-full bg-slate-800/80 rounded-full h-2 overflow-hidden p-0.5 border border-slate-700/50">
            <div
              className={`h-full rounded-full transition-all duration-300 ${barColor}`}
              style={{ width: `${pct}%` }}
            />
          </div>
        </div>
      </td>
      <td className="py-3.5 px-4">
        {lane.hasEmergency ? (
          <span className="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-md text-xs font-semibold bg-rose-500/20 text-rose-400 border border-rose-500/40 animate-pulse">
            <ShieldAlert className="w-3.5 h-3.5" />
            <span>PREEMPTION ACTIVE</span>
          </span>
        ) : (
          <span className="inline-flex items-center gap-1 text-xs text-slate-500 font-mono">
            <CheckCircle2 className="w-3.5 h-3.5 text-slate-600" />
            <span>Normal Flow</span>
          </span>
        )}
      </td>
      <td className="py-3.5 px-4 text-right">
        <div className="text-xs font-mono text-slate-400">
          {lane.vehicles.length > 0 ? (
            <span>Head ID: <span className="text-cyan-400 font-bold">#{lane.vehicles[0].id}</span></span>
          ) : (
            <span className="text-slate-600">—</span>
          )}
        </div>
      </td>
    </tr>
  )
}

export default function Dashboard() {
  const { state, connected, error, history, fetchState, sendControl, isControlling } = useSimStore()

  useEffect(() => {
    fetchState()
    const interval = setInterval(fetchState, 1000)
    return () => clearInterval(interval)
  }, [fetchState])

  const totalWaiting = state?.lanes.reduce((sum, l) => sum + l.queueSize, 0) ?? 0
  const emergencyCount = state?.lanes.filter((l) => l.hasEmergency).length ?? 0
  const totalVehiclesActive = (state?.stats.totalGenerated ?? 0) - (state?.stats.totalProcessed ?? 0)

  const laneBarData = (state?.lanes ?? []).map((l) => ({
    name: l.name.replace('_Bound', ''),
    queue: l.queueSize,
    capacity: l.capacity,
    available: Math.max(0, l.capacity - l.queueSize),
  }))

  return (
    <div className="min-h-screen bg-[#060810] text-slate-100 cyber-grid p-4 md:p-8 space-y-6">
      {/* Top Header & Simulation Controls */}
      <div className="glass-panel rounded-2xl p-6 flex flex-col lg:flex-row items-start lg:items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-2xl md:text-3xl font-extrabold tracking-tight bg-gradient-to-r from-white via-slate-200 to-cyan-400 bg-clip-text text-transparent">
              Traffic Intelligence Center
            </h1>
            <span className="px-2 py-0.5 rounded text-[10px] font-mono font-semibold bg-blue-500/10 text-cyan-400 border border-cyan-500/30">
              C++ ENGINE BRIDGE
            </span>
          </div>
          <p className="text-sm text-slate-400 mt-1">
            Real-time multi-lane queue metrics, emergency vehicle preemption, and signal state telemetry.
          </p>
        </div>

        {/* Live Controls */}
        <div className="flex flex-wrap items-center gap-2">
          {state?.status === 'running' ? (
            <button
              onClick={() => sendControl('pause')}
              disabled={isControlling}
              className="flex items-center gap-2 px-4 py-2 rounded-xl text-xs font-semibold bg-amber-500/10 border border-amber-500/30 text-amber-300 hover:bg-amber-500/20 transition-all shadow-[0_0_15px_rgba(245,158,11,0.15)] disabled:opacity-50"
            >
              <Pause className="w-3.5 h-3.5" />
              <span>Pause</span>
            </button>
          ) : (
            <button
              onClick={() => sendControl('resume')}
              disabled={isControlling}
              className="flex items-center gap-2 px-4 py-2 rounded-xl text-xs font-semibold bg-emerald-500/10 border border-emerald-500/30 text-emerald-300 hover:bg-emerald-500/20 transition-all shadow-[0_0_15px_rgba(16,185,129,0.2)] disabled:opacity-50"
            >
              <Play className="w-3.5 h-3.5" />
              <span>{state?.status === 'paused' ? 'Resume' : 'Start'}</span>
            </button>
          )}

          <button
            onClick={() => sendControl('reset')}
            disabled={isControlling}
            className="flex items-center gap-2 px-3 py-2 rounded-xl text-xs font-semibold bg-slate-800 border border-slate-700 text-slate-300 hover:text-white hover:bg-slate-700 transition-all disabled:opacity-50"
          >
            <RotateCcw className="w-3.5 h-3.5" />
            <span>Reset Engine</span>
          </button>
        </div>
      </div>

      {/* Offline Alert if disconnected */}
      {!connected && (
        <div className="glass-panel border-rose-500/40 bg-rose-950/20 rounded-xl p-4 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <AlertTriangle className="w-5 h-5 text-rose-400 animate-pulse" />
            <div>
              <div className="text-sm font-semibold text-rose-300">Local C++ Bridge Offline</div>
              <div className="text-xs text-rose-400/80">
                Ensure <code className="bg-rose-900/40 px-1 py-0.5 rounded font-mono">./build/traffic-bridge</code> is running on port 8080.
                {error && <span className="ml-1">({error})</span>}
              </div>
            </div>
          </div>
          <button
            onClick={() => fetchState()}
            className="px-3 py-1.5 rounded-lg text-xs font-semibold bg-rose-500/20 hover:bg-rose-500/30 text-rose-200 border border-rose-500/30 transition-all"
          >
            Retry Connection
          </button>
        </div>
      )}

      {/* KPI Stat Cards Grid */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        {/* Total Generated */}
        <div className="glass-panel rounded-2xl p-5 border-l-4 border-l-cyber-blue relative overflow-hidden group hover:border-cyber-cyan/50 transition-all">
          <div className="flex justify-between items-start">
            <div>
              <span className="text-xs font-mono font-medium uppercase tracking-wider text-slate-400">Total Generated</span>
              <div className="text-3xl font-extrabold text-white font-mono mt-1">
                {state?.stats.totalGenerated ?? '—'}
              </div>
            </div>
            <div className="p-2.5 rounded-xl bg-blue-500/10 border border-blue-500/20 text-cyber-cyan group-hover:scale-110 transition-transform">
              <Car className="w-5 h-5" />
            </div>
          </div>
          <div className="mt-3 flex items-center gap-1.5 text-xs text-slate-400">
            <span className="text-emerald-400 font-semibold flex items-center">
              <ArrowUpRight className="w-3.5 h-3.5" /> Live
            </span>
            <span>Spawn rate 0.40 - 0.65/tick</span>
          </div>
        </div>

        {/* Total Processed (Throughput) */}
        <div className="glass-panel rounded-2xl p-5 border-l-4 border-l-cyber-cyan relative overflow-hidden group hover:border-cyber-cyan/50 transition-all">
          <div className="flex justify-between items-start">
            <div>
              <span className="text-xs font-mono font-medium uppercase tracking-wider text-slate-400">Total Processed</span>
              <div className="text-3xl font-extrabold text-white font-mono mt-1 text-cyber-cyan">
                {state?.stats.totalProcessed ?? '—'}
              </div>
            </div>
            <div className="p-2.5 rounded-xl bg-cyan-500/10 border border-cyan-500/20 text-cyber-cyan group-hover:scale-110 transition-transform">
              <CheckCircle2 className="w-5 h-5" />
            </div>
          </div>
          <div className="mt-3 flex items-center gap-1.5 text-xs text-slate-400">
            <span className="text-cyan-400 font-mono">
              {state?.stats.totalGenerated ? Math.round(((state.stats.totalProcessed) / (state.stats.totalGenerated || 1)) * 100) : 0}%
            </span>
            <span>intersection clearance rate</span>
          </div>
        </div>

        {/* Waiting Vehicles */}
        <div className="glass-panel rounded-2xl p-5 border-l-4 border-l-amber-500 relative overflow-hidden group hover:border-amber-500/50 transition-all">
          <div className="flex justify-between items-start">
            <div>
              <span className="text-xs font-mono font-medium uppercase tracking-wider text-slate-400">Waiting Vehicles</span>
              <div className="text-3xl font-extrabold text-white font-mono mt-1 text-amber-300">
                {totalWaiting}
              </div>
            </div>
            <div className="p-2.5 rounded-xl bg-amber-500/10 border border-amber-500/20 text-amber-400 group-hover:scale-110 transition-transform">
              <Clock className="w-5 h-5" />
            </div>
          </div>
          <div className="mt-3 flex items-center gap-1.5 text-xs text-slate-400">
            <span className="font-mono text-amber-300">{totalVehiclesActive}</span>
            <span>active across all 4 road approaches</span>
          </div>
        </div>

        {/* Emergency Vehicles */}
        <div className={`glass-panel rounded-2xl p-5 border-l-4 ${emergencyCount > 0 ? 'border-l-rose-500 shadow-glow-red' : 'border-l-slate-700'} relative overflow-hidden group transition-all`}>
          <div className="flex justify-between items-start">
            <div>
              <span className="text-xs font-mono font-medium uppercase tracking-wider text-slate-400">Emergency Vehicles</span>
              <div className={`text-3xl font-extrabold font-mono mt-1 ${emergencyCount > 0 ? 'text-rose-400 animate-pulse' : 'text-slate-300'}`}>
                {emergencyCount}
              </div>
            </div>
            <div className={`p-2.5 rounded-xl ${emergencyCount > 0 ? 'bg-rose-500/20 border border-rose-500/40 text-rose-400 animate-bounce' : 'bg-slate-800 text-slate-500'}`}>
              <ShieldAlert className="w-5 h-5" />
            </div>
          </div>
          <div className="mt-3 flex items-center gap-1.5 text-xs text-slate-400">
            <span className="font-mono text-rose-400 font-semibold">{state?.stats.totalPreemptions ?? 0}</span>
            <span>lifetime preemptions triggered</span>
          </div>
        </div>
      </div>

      {/* Secondary Metrics & Quick Signals Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-4 gap-4">
        {/* Signal Status Cards (4 approaches) */}
        {(state?.lanes ?? []).map((lane) => {
          const isGreen = lane.signal === 'GREEN'
          const isYellow = lane.signal === 'YELLOW'
          return (
            <div
              key={lane.name}
              className={`glass-panel rounded-2xl p-4 border transition-all ${
                isGreen
                  ? 'border-emerald-500/40 bg-emerald-950/10 shadow-[0_0_20px_rgba(16,185,129,0.15)]'
                  : isYellow
                  ? 'border-amber-500/40 bg-amber-950/10 shadow-[0_0_20px_rgba(245,158,11,0.15)]'
                  : 'border-slate-800/80 bg-slate-900/40'
              }`}
            >
              <div className="flex items-center justify-between mb-2">
                <span className="text-xs font-mono text-slate-400 uppercase tracking-wider">
                  {lane.name.replace('_Bound', ' Bound')}
                </span>
                <SignalBadge signal={lane.signal} />
              </div>
              <div className="flex items-baseline justify-between mt-3">
                <div className="text-2xl font-bold font-mono text-white">
                  {lane.queueSize} <span className="text-xs text-slate-500 font-normal">queued</span>
                </div>
                <div className="text-xs font-mono text-slate-400">
                  Cap: {lane.capacity}
                </div>
              </div>
              <div className="mt-2 w-full bg-slate-800 rounded-full h-1.5 overflow-hidden">
                <div
                  className={`h-full ${isGreen ? 'bg-emerald-400' : isYellow ? 'bg-amber-400' : 'bg-rose-500'}`}
                  style={{ width: `${Math.min(100, (lane.queueSize / lane.capacity) * 100)}%` }}
                />
              </div>
            </div>
          )
        })}
      </div>

      {/* Charts Section */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Live Traffic Flow Area Chart (2 cols) */}
        <div className="lg:col-span-2 glass-panel rounded-2xl p-6 space-y-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-2">
              <Activity className="w-4 h-4 text-cyber-cyan" />
              <h2 className="text-sm font-bold uppercase tracking-wider text-slate-200">
                Real-Time Flow Dynamics (Last 60 Ticks)
              </h2>
            </div>
            <div className="text-xs font-mono text-slate-400">
              Current Tick: <span className="text-cyber-cyan font-bold">#{state?.tick ?? 0}</span>
            </div>
          </div>

          <div className="h-64 w-full">
            <ResponsiveContainer width="100%" height="100%">
              <AreaChart data={history} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <defs>
                  <linearGradient id="gradientProcessed" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor="#00d4ff" stopOpacity={0.35} />
                    <stop offset="95%" stopColor="#00d4ff" stopOpacity={0.0} />
                  </linearGradient>
                  <linearGradient id="gradientWaiting" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor="#1d6fe8" stopOpacity={0.4} />
                    <stop offset="95%" stopColor="#1d6fe8" stopOpacity={0.0} />
                  </linearGradient>
                </defs>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2744" vertical={false} />
                <XAxis dataKey="tick" stroke="#64748b" tick={{ fontSize: 10, fill: '#64748b' }} />
                <YAxis stroke="#64748b" tick={{ fontSize: 10, fill: '#64748b' }} />
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#0d1424',
                    border: '1px solid rgba(0,212,255,0.3)',
                    borderRadius: '12px',
                    boxShadow: '0 8px 24px rgba(0,0,0,0.6)',
                    fontSize: '12px',
                    fontFamily: 'JetBrains Mono',
                  }}
                />
                <Legend wrapperStyle={{ fontSize: '11px', paddingTop: '10px' }} />
                <Area
                  type="monotone"
                  dataKey="processed"
                  stroke="#00d4ff"
                  strokeWidth={2}
                  fill="url(#gradientProcessed)"
                  name="Processed Throughput"
                />
                <Area
                  type="monotone"
                  dataKey="waiting"
                  stroke="#1d6fe8"
                  strokeWidth={2}
                  fill="url(#gradientWaiting)"
                  name="Queued Vehicles"
                />
              </AreaChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Lane Congestion Distribution (1 col) */}
        <div className="glass-panel rounded-2xl p-6 space-y-4">
          <div className="flex items-center gap-2">
            <Layers className="w-4 h-4 text-cyber-blue" />
            <h2 className="text-sm font-bold uppercase tracking-wider text-slate-200">
              Lane Queue Load
            </h2>
          </div>
          <p className="text-xs text-slate-400">
            Active queue count vs residual buffer capacity per intersection direction.
          </p>

          <div className="h-64 w-full">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={laneBarData} layout="vertical" margin={{ top: 5, right: 10, left: 10, bottom: 5 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2744" horizontal={false} />
                <XAxis type="number" stroke="#64748b" tick={{ fontSize: 10 }} />
                <YAxis dataKey="name" type="category" stroke="#94a3b8" tick={{ fontSize: 11 }} />
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#0d1424',
                    border: '1px solid #1a2744',
                    borderRadius: '8px',
                    fontSize: '11px',
                  }}
                />
                <Legend wrapperStyle={{ fontSize: '11px' }} />
                <Bar dataKey="queue" name="Waiting" stackId="a" fill="#00d4ff" radius={[0, 0, 0, 0]} />
                <Bar dataKey="available" name="Free Capacity" stackId="a" fill="#1a2744" radius={[0, 4, 4, 0]} />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>
      </div>

      {/* Lane-wise Congestion Detailed Table */}
      <div className="glass-panel rounded-2xl overflow-hidden border border-cyber-border">
        <div className="p-5 border-b border-slate-800/80 flex items-center justify-between">
          <div className="flex items-center gap-2">
            <Zap className="w-4 h-4 text-cyber-cyan" />
            <h2 className="text-base font-bold text-white tracking-wide">
              Lane-Wise Congestion & Real-Time Telemetry
            </h2>
          </div>
          <span className="text-xs font-mono text-slate-400">
            4 Configured Lanes · Capacity 15 per approach
          </span>
        </div>

        <div className="overflow-x-auto">
          <table className="w-full text-left">
            <thead>
              <tr className="bg-slate-900/60 border-b border-slate-800/80 text-[11px] font-mono text-slate-400 uppercase tracking-wider">
                <th className="py-3 px-4">Lane Direction</th>
                <th className="py-3 px-4">Active Signal</th>
                <th className="py-3 px-4">Queue Congestion (Depth / Cap)</th>
                <th className="py-3 px-4">Preemption / Status</th>
                <th className="py-3 px-4 text-right">Queue Lead</th>
              </tr>
            </thead>
            <tbody>
              {(state?.lanes ?? []).map((lane) => (
                <LaneRow key={lane.name} lane={lane} />
              ))}
              {!state && (
                <tr>
                  <td colSpan={5} className="text-center py-12 text-slate-500 font-mono text-sm">
                    Connecting to C++ simulation engine bridge...
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>

      {/* Bottom KPI Bar */}
      {state && (
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4 text-center">
          <div className="glass-panel rounded-xl p-4">
            <span className="text-[11px] font-mono uppercase tracking-wider text-slate-400">Average Wait Time</span>
            <div className="text-2xl font-bold font-mono text-cyber-cyan mt-1">
              {state.stats.avgWaitTime.toFixed(2)} <span className="text-xs text-slate-400 font-normal">ticks</span>
            </div>
          </div>
          <div className="glass-panel rounded-xl p-4">
            <span className="text-[11px] font-mono uppercase tracking-wider text-slate-400">Overflow Blocked Vehicles</span>
            <div className="text-2xl font-bold font-mono text-rose-400 mt-1">
              {state.stats.totalBlocked}
            </div>
          </div>
          <div className="glass-panel rounded-xl p-4">
            <span className="text-[11px] font-mono uppercase tracking-wider text-slate-400">Total Preemption Events</span>
            <div className="text-2xl font-bold font-mono text-amber-300 mt-1">
              {state.stats.totalPreemptions}
            </div>
          </div>
        </div>
      )}
    </div>
  )
}
