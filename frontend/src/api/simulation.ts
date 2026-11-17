import axios from 'axios'

export interface VehicleData {
  id: number
  isEmergency: boolean
  arrivalTick: number
}

export interface LaneData {
  name: string
  signal: 'RED' | 'YELLOW' | 'GREEN'
  queueSize: number
  capacity: number
  hasEmergency: boolean
  vehicles: VehicleData[]
}

export interface SimStats {
  totalGenerated: number
  totalProcessed: number
  totalBlocked: number
  totalPreemptions: number
  avgWaitTime: number
}

export interface SimState {
  tick: number
  status: 'running' | 'paused' | 'stopped'
  stats: SimStats
  lanes: LaneData[]
}

// Support both direct proxy /api and fallback to localhost:8080 directly
const client = axios.create({
  baseURL: '/api',
  timeout: 3000,
})

export const fetchSimulationState = async (): Promise<SimState> => {
  try {
    const res = await client.get<SimState>('/state')
    return res.data
  } catch {
    // Direct fallback if proxy is bypassed
    const directRes = await axios.get<SimState>('http://127.0.0.1:8080/state', { timeout: 3000 })
    return directRes.data
  }
}

export const postSimulationControl = async (action: 'pause' | 'resume' | 'stop' | 'reset'): Promise<boolean> => {
  try {
    const res = await client.post<{ ok: boolean }>('/control', { action })
    return res.data.ok
  } catch {
    const directRes = await axios.post<{ ok: boolean }>('http://127.0.0.1:8080/control', { action }, { timeout: 3000 })
    return directRes.data.ok
  }
}
