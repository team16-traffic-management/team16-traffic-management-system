import { create } from 'zustand'
import { SimState, fetchSimulationState, postSimulationControl } from '../api/simulation'

export interface HistoryPoint {
  tick: number
  processed: number
  waiting: number
  generated: number
  blocked: number
}

interface SimStoreState {
  state: SimState | null
  connected: boolean
  lastUpdated: number | null
  error: string | null
  history: HistoryPoint[]
  autoRefresh: boolean
  isControlling: boolean

  fetchState: () => Promise<void>
  sendControl: (action: 'pause' | 'resume' | 'stop' | 'reset') => Promise<void>
  setAutoRefresh: (enabled: boolean) => void
}

export const useSimStore = create<SimStoreState>((set, get) => ({
  state: null,
  connected: false,
  lastUpdated: null,
  error: null,
  history: [],
  autoRefresh: true,
  isControlling: false,

  fetchState: async () => {
    try {
      const data = await fetchSimulationState()
      const prevHistory = get().history
      const totalWaiting = data.lanes.reduce((sum, l) => sum + l.queueSize, 0)

      // Only push to history if tick changed
      let nextHistory = prevHistory
      const lastTick = prevHistory.length > 0 ? prevHistory[prevHistory.length - 1].tick : -1
      if (data.tick !== lastTick) {
        nextHistory = [
          ...prevHistory.slice(-59), // keep last 60 ticks
          {
            tick: data.tick,
            processed: data.stats.totalProcessed,
            waiting: totalWaiting,
            generated: data.stats.totalGenerated,
            blocked: data.stats.totalBlocked,
          },
        ]
      }

      set({
        state: data,
        connected: true,
        lastUpdated: Date.now(),
        error: null,
        history: nextHistory,
      })
    } catch (err: unknown) {
      const msg = err instanceof Error ? err.message : 'Bridge unreachable'
      set({
        connected: false,
        error: msg,
      })
    }
  },

  sendControl: async (action: 'pause' | 'resume' | 'stop' | 'reset') => {
    set({ isControlling: true })
    try {
      await postSimulationControl(action)
      // immediately refresh state after control command
      await get().fetchState()
    } catch (err: unknown) {
      const msg = err instanceof Error ? err.message : 'Control failed'
      set({ error: msg })
    } finally {
      set({ isControlling: false })
    }
  },

  setAutoRefresh: (enabled: boolean) => set({ autoRefresh: enabled }),
}))
