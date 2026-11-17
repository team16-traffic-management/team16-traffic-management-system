import { Link, useLocation } from 'react-router-dom'
import { Activity, Box, Cpu, RefreshCw } from 'lucide-react'
import { useSimStore } from '../store/simStore'

export default function NavBar() {
  const location = useLocation()
  const { connected, state, isControlling, fetchState } = useSimStore()

  const navLinks = [
    { path: '/', label: 'Dashboard', icon: Activity },
    { path: '/simulation', label: 'Live 3D Simulation', icon: Box },
  ]

  return (
    <header className="fixed top-0 left-0 right-0 z-50 h-16 glass-panel border-b border-cyber-border bg-[#060810]/85 px-4 md:px-8 flex items-center justify-between">
      {/* Brand */}
      <div className="flex items-center gap-3">
        <div className="relative flex items-center justify-center w-10 h-10 rounded-xl bg-gradient-to-tr from-cyber-blue to-cyber-cyan p-[1px] shadow-glow-cyan">
          <div className="w-full h-full bg-[#0d1424] rounded-xl flex items-center justify-center">
            <Cpu className="w-5 h-5 text-cyber-cyan animate-pulse" />
          </div>
        </div>
        <div>
          <div className="flex items-center gap-2">
            <span className="font-bold tracking-wider text-white text-base">TEAM 16</span>
            <span className="text-[10px] font-mono px-1.5 py-0.5 rounded bg-cyan-500/10 border border-cyan-500/30 text-cyber-cyan">TMS v2.0</span>
          </div>
          <div className="text-[11px] font-mono text-slate-400">SMART-CITY TRAFFIC ENGINE</div>
        </div>
      </div>

      {/* Center Nav tabs: ONLY TWO PAGES */}
      <nav className="flex items-center p-1 rounded-xl bg-slate-900/80 border border-slate-800">
        {navLinks.map(({ path, label, icon: Icon }) => {
          const isActive = location.pathname === path
          return (
            <Link
              key={path}
              to={path}
              className={`flex items-center gap-2 px-4 py-1.5 rounded-lg text-xs md:text-sm font-medium transition-all duration-200 ${
                isActive
                  ? 'bg-gradient-to-r from-blue-600/30 to-cyan-500/30 text-cyber-cyan border border-cyber-cyan/40 shadow-glow-cyan font-semibold'
                  : 'text-slate-400 hover:text-slate-200 hover:bg-slate-800/40 border border-transparent'
              }`}
            >
              <Icon className={`w-4 h-4 ${isActive ? 'text-cyber-cyan' : 'text-slate-500'}`} />
              <span>{label}</span>
            </Link>
          )
        })}
      </nav>

      {/* Right status */}
      <div className="flex items-center gap-3">
        {/* Tick Badge */}
        {state && (
          <div className="hidden sm:flex items-center gap-2 px-3 py-1 rounded-lg bg-cyber-panel border border-slate-800">
            <span className="text-[10px] font-mono text-slate-400 uppercase">TICK</span>
            <span className="text-sm font-mono font-bold text-cyber-cyan">#{state.tick}</span>
          </div>
        )}

        {/* Engine Status */}
        <div
          className={`flex items-center gap-2 px-3 py-1 rounded-full text-xs font-mono border ${
            connected
              ? state?.status === 'running'
                ? 'border-emerald-500/40 bg-emerald-500/10 text-emerald-400'
                : 'border-amber-500/40 bg-amber-500/10 text-amber-400'
              : 'border-rose-500/40 bg-rose-500/10 text-rose-400'
          }`}
        >
          <span
            className={`w-2 h-2 rounded-full ${
              connected
                ? state?.status === 'running'
                  ? 'bg-emerald-400 animate-pulse'
                  : 'bg-amber-400'
                : 'bg-rose-500'
            }`}
          />
          <span className="uppercase font-semibold">
            {connected ? (state ? state.status : 'ONLINE') : 'DISCONNECTED'}
          </span>
        </div>

        {/* Manual Refresh */}
        <button
          onClick={() => fetchState()}
          disabled={isControlling}
          title="Refresh Data"
          className="p-1.5 rounded-lg bg-slate-900 border border-slate-800 text-slate-400 hover:text-white hover:border-cyber-border transition-colors disabled:opacity-50"
        >
          <RefreshCw className={`w-4 h-4 ${isControlling ? 'animate-spin text-cyber-cyan' : ''}`} />
        </button>
      </div>
    </header>
  )
}
