import { useEffect, useRef, useState } from 'react'
import { Canvas, useFrame } from '@react-three/fiber'
import { OrbitControls, Text } from '@react-three/drei'
import type { OrbitControls as OrbitControlsImpl } from 'three-stdlib'
import * as THREE from 'three'
import {
  Camera,
  Compass,
  Maximize2,
  Pause,
  Play,
  RotateCcw,
  ShieldAlert,
  Sliders,
  Sun,
  Video,
} from 'lucide-react'
import { useSimStore } from '../store/simStore'
import type { LaneData, VehicleData } from '../api/simulation'

// ---------------------------------------------------------------------------
// 3D Road Infrastructure & Ground Components
// ---------------------------------------------------------------------------

function RoadIntersection() {
  return (
    <group>
      {/* Surrounding landscape */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.06, 0]} receiveShadow>
        <planeGeometry args={[140, 140]} />
        <meshStandardMaterial color="#080c14" roughness={0.95} metalness={0.1} />
      </mesh>

      {/* Cyber ground grid rings */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.055, 0]}>
        <ringGeometry args={[26, 26.3, 64]} />
        <meshBasicMaterial color="#1d6fe8" transparent opacity={0.15} />
      </mesh>
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.055, 0]}>
        <ringGeometry args={[44, 44.3, 64]} />
        <meshBasicMaterial color="#00d4ff" transparent opacity={0.1} />
      </mesh>

      {/* Center Intersection Box (12m x 12m) */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.02, 0]} receiveShadow>
        <planeGeometry args={[12, 12]} />
        <meshStandardMaterial color="#141824" roughness={0.8} />
      </mesh>

      {/* Road Arms (Asphalt, width 12m, length 40m) */}
      {/* North arm (+Z) */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.02, 26]} receiveShadow>
        <planeGeometry args={[12, 40]} />
        <meshStandardMaterial color="#121620" roughness={0.85} />
      </mesh>
      {/* South arm (-Z) */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.02, -26]} receiveShadow>
        <planeGeometry args={[12, 40]} />
        <meshStandardMaterial color="#121620" roughness={0.85} />
      </mesh>
      {/* East arm (+X) */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[26, -0.02, 0]} receiveShadow>
        <planeGeometry args={[40, 12]} />
        <meshStandardMaterial color="#121620" roughness={0.85} />
      </mesh>
      {/* West arm (-X) */}
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[-26, -0.02, 0]} receiveShadow>
        <planeGeometry args={[40, 12]} />
        <meshStandardMaterial color="#121620" roughness={0.85} />
      </mesh>

      {/* Sidewalks / Curbs (Elevated grey curbs) */}
      {/* North-East corner */}
      <mesh position={[9.5, 0.1, 9.5]}>
        <boxGeometry args={[7, 0.25, 7]} />
        <meshStandardMaterial color="#1c2538" roughness={0.7} />
      </mesh>
      {/* North-West corner */}
      <mesh position={[-9.5, 0.1, 9.5]}>
        <boxGeometry args={[7, 0.25, 7]} />
        <meshStandardMaterial color="#1c2538" roughness={0.7} />
      </mesh>
      {/* South-East corner */}
      <mesh position={[9.5, 0.1, -9.5]}>
        <boxGeometry args={[7, 0.25, 7]} />
        <meshStandardMaterial color="#1c2538" roughness={0.7} />
      </mesh>
      {/* South-West corner */}
      <mesh position={[-9.5, 0.1, -9.5]}>
        <boxGeometry args={[7, 0.25, 7]} />
        <meshStandardMaterial color="#1c2538" roughness={0.7} />
      </mesh>
    </group>
  )
}

function RoadMarkings() {
  const dashes: JSX.Element[] = []

  // Dashed lane dividers along North & South (+/- Z)
  for (let z = 8; z <= 44; z += 3) {
    // North lane divider
    dashes.push(
      <mesh key={`nd-${z}`} rotation={[-Math.PI / 2, 0, 0]} position={[0, 0.005, z]}>
        <planeGeometry args={[0.2, 1.6]} />
        <meshBasicMaterial color="#ffffff" transparent opacity={0.65} />
      </mesh>
    )
    // South lane divider
    dashes.push(
      <mesh key={`sd-${z}`} rotation={[-Math.PI / 2, 0, 0]} position={[0, 0.005, -z]}>
        <planeGeometry args={[0.2, 1.6]} />
        <meshBasicMaterial color="#ffffff" transparent opacity={0.65} />
      </mesh>
    )
  }

  // Dashed lane dividers along East & West (+/- X)
  for (let x = 8; x <= 44; x += 3) {
    // East lane divider
    dashes.push(
      <mesh key={`ed-${x}`} rotation={[-Math.PI / 2, 0, 0]} position={[x, 0.005, 0]}>
        <planeGeometry args={[1.6, 0.2]} />
        <meshBasicMaterial color="#ffffff" transparent opacity={0.65} />
      </mesh>
    )
    // West lane divider
    dashes.push(
      <mesh key={`wd-${x}`} rotation={[-Math.PI / 2, 0, 0]} position={[-x, 0.005, 0]}>
        <planeGeometry args={[1.6, 0.2]} />
        <meshBasicMaterial color="#ffffff" transparent opacity={0.65} />
      </mesh>
    )
  }

  // Solid Stop Bars at intersection entries (6m from center)
  const stopBars = [
    // North approach stop bar
    <mesh key="sb-n" rotation={[-Math.PI / 2, 0, 0]} position={[3, 0.01, 6.2]}>
      <planeGeometry args={[5.8, 0.6]} />
      <meshBasicMaterial color="#ffffff" />
    </mesh>,
    // South approach stop bar
    <mesh key="sb-s" rotation={[-Math.PI / 2, 0, 0]} position={[-3, 0.01, -6.2]}>
      <planeGeometry args={[5.8, 0.6]} />
      <meshBasicMaterial color="#ffffff" />
    </mesh>,
    // East approach stop bar
    <mesh key="sb-e" rotation={[-Math.PI / 2, 0, 0]} position={[6.2, 0.01, -3]}>
      <planeGeometry args={[0.6, 5.8]} />
      <meshBasicMaterial color="#ffffff" />
    </mesh>,
    // West approach stop bar
    <mesh key="sb-w" rotation={[-Math.PI / 2, 0, 0]} position={[-6.2, 0.01, 3]}>
      <planeGeometry args={[0.6, 5.8]} />
      <meshBasicMaterial color="#ffffff" />
    </mesh>,
  ]

  // Crosswalk Stripes (Zebra crossings)
  const crosswalks: JSX.Element[] = []
  for (let offset = -5; offset <= 5; offset += 1.2) {
    // North Crosswalk
    crosswalks.push(
      <mesh key={`cwn-${offset}`} rotation={[-Math.PI / 2, 0, 0]} position={[offset, 0.008, 5.2]}>
        <planeGeometry args={[0.6, 1.2]} />
        <meshBasicMaterial color="#cbd5e1" transparent opacity={0.7} />
      </mesh>
    )
    // South Crosswalk
    crosswalks.push(
      <mesh key={`cws-${offset}`} rotation={[-Math.PI / 2, 0, 0]} position={[offset, 0.008, -5.2]}>
        <planeGeometry args={[0.6, 1.2]} />
        <meshBasicMaterial color="#cbd5e1" transparent opacity={0.7} />
      </mesh>
    )
    // East Crosswalk
    crosswalks.push(
      <mesh key={`cwe-${offset}`} rotation={[-Math.PI / 2, 0, 0]} position={[5.2, 0.008, offset]}>
        <planeGeometry args={[1.2, 0.6]} />
        <meshBasicMaterial color="#cbd5e1" transparent opacity={0.7} />
      </mesh>
    )
    // West Crosswalk
    crosswalks.push(
      <mesh key={`cww-${offset}`} rotation={[-Math.PI / 2, 0, 0]} position={[-5.2, 0.008, offset]}>
        <planeGeometry args={[1.2, 0.6]} />
        <meshBasicMaterial color="#cbd5e1" transparent opacity={0.7} />
      </mesh>
    )
  }

  return (
    <group>
      {dashes}
      {stopBars}
      {crosswalks}
    </group>
  )
}

// ---------------------------------------------------------------------------
// 3D Traffic Signal Gantry & Light
// ---------------------------------------------------------------------------

function TrafficLightGantry({
  position,
  rotationY,
  signal,
  laneName,
}: {
  position: [number, number, number]
  rotationY: number
  signal: 'RED' | 'YELLOW' | 'GREEN'
  laneName: string
}) {
  const isGreen = signal === 'GREEN'
  const isYellow = signal === 'YELLOW'
  const isRed = signal === 'RED' || (!isGreen && !isYellow)

  return (
    <group position={position} rotation={[0, rotationY, 0]}>
      {/* Vertical Steel Mast Pole */}
      <mesh position={[0, 2.5, 0]} castShadow>
        <cylinderGeometry args={[0.14, 0.18, 5.0, 16]} />
        <meshStandardMaterial color="#334155" metalness={0.7} roughness={0.3} />
      </mesh>

      {/* Horizontal Cantilever Arm extending toward the lane */}
      <mesh position={[1.4, 4.8, 0]} rotation={[0, 0, -Math.PI / 2]} castShadow>
        <cylinderGeometry args={[0.1, 0.12, 3.2, 16]} />
        <meshStandardMaterial color="#334155" metalness={0.7} roughness={0.3} />
      </mesh>

      {/* Traffic Light Signal Head Assembly */}
      <group position={[2.6, 4.3, 0]}>
        {/* Signal Housing Box */}
        <mesh castShadow>
          <boxGeometry args={[0.6, 1.8, 0.35]} />
          <meshStandardMaterial color="#0f172a" roughness={0.4} />
        </mesh>
        {/* Backplate / Border */}
        <mesh position={[0, 0, -0.05]}>
          <boxGeometry args={[0.8, 2.0, 0.05]} />
          <meshStandardMaterial color="#020617" roughness={0.6} />
        </mesh>

        {/* RED Lens (Top) */}
        <mesh position={[0, 0.55, 0.18]}>
          <sphereGeometry args={[0.18, 20, 20]} />
          <meshStandardMaterial
            color={isRed ? '#ff1e42' : '#3d0a14'}
            emissive={isRed ? '#ff0033' : '#000000'}
            emissiveIntensity={isRed ? 2.5 : 0}
            roughness={0.2}
          />
        </mesh>
        {isRed && (
          <pointLight position={[0, 0.55, 0.6]} color="#ff1e42" intensity={2.2} distance={8} decay={2} />
        )}

        {/* YELLOW Lens (Middle) */}
        <mesh position={[0, 0, 0.18]}>
          <sphereGeometry args={[0.18, 20, 20]} />
          <meshStandardMaterial
            color={isYellow ? '#ffb703' : '#3d2800'}
            emissive={isYellow ? '#ffa200' : '#000000'}
            emissiveIntensity={isYellow ? 2.5 : 0}
            roughness={0.2}
          />
        </mesh>
        {isYellow && (
          <pointLight position={[0, 0, 0.6]} color="#ffb703" intensity={2.2} distance={8} decay={2} />
        )}

        {/* GREEN Lens (Bottom) */}
        <mesh position={[0, -0.55, 0.18]}>
          <sphereGeometry args={[0.18, 20, 20]} />
          <meshStandardMaterial
            color={isGreen ? '#00f59b' : '#00331e'}
            emissive={isGreen ? '#00f59b' : '#000000'}
            emissiveIntensity={isGreen ? 2.5 : 0}
            roughness={0.2}
          />
        </mesh>
        {isGreen && (
          <pointLight position={[0, -0.55, 0.6]} color="#00f59b" intensity={2.5} distance={8} decay={2} />
        )}

        {/* Direction Text Tag */}
        <Text
          position={[0, -1.2, 0.2]}
          fontSize={0.28}
          color="#38bdf8"
          anchorX="center"
          anchorY="middle"
        >
          {laneName}
        </Text>
      </group>
    </group>
  )
}

// ---------------------------------------------------------------------------
// 3D Moving Vehicle Component
// ---------------------------------------------------------------------------

function SimulationVehicle({
  vehicle,
  queueIndex,
  direction,
  signal,
  tick,
}: {
  vehicle: VehicleData
  queueIndex: number
  direction: 'North_Bound' | 'South_Bound' | 'East_Bound' | 'West_Bound'
  signal: 'RED' | 'YELLOW' | 'GREEN'
  tick: number
}) {
  const meshRef = useRef<THREE.Group>(null)
  const isEmergency = vehicle.isEmergency

  // Distance from stop line (stop line is at 6.5m from center)
  // Each queued car is spaced by 4.2m
  const baseStopDistance = 7.5 + queueIndex * 4.2

  // When signal is GREEN, vehicles advance toward and through the intersection
  // We compute a smooth tick offset animation
  const greenAdvance = signal === 'GREEN' ? ((tick * 1.5) % 15) : 0
  const effectiveDistance = Math.max(1.5, baseStopDistance - greenAdvance)

  // Calculate 3D position and rotation according to approach direction
  // Lanes drive on the right side of the road (lateral offset ~ 3.0m)
  let pos: [number, number, number] = [0, 0, 0]
  let rotY = 0

  switch (direction) {
    case 'North_Bound':
      // Coming from +Z traveling North toward -Z
      pos = [3.0, 0.45, effectiveDistance]
      rotY = Math.PI
      break
    case 'South_Bound':
      // Coming from -Z traveling South toward +Z
      pos = [-3.0, 0.45, -effectiveDistance]
      rotY = 0
      break
    case 'East_Bound':
      // Coming from +X traveling West toward -X
      pos = [effectiveDistance, 0.45, -3.0]
      rotY = -Math.PI / 2
      break
    case 'West_Bound':
      // Coming from -X traveling East toward +X
      pos = [-effectiveDistance, 0.45, 3.0]
      rotY = Math.PI / 2
      break
  }

  // Emergency flashing lights animation
  const strobeLightRef = useRef<THREE.PointLight>(null)
  useFrame((state) => {
    if (isEmergency && strobeLightRef.current) {
      const flash = Math.sin(state.clock.elapsedTime * 12) > 0
      strobeLightRef.current.intensity = flash ? 3.0 : 0.2
    }
  })

  return (
    <group ref={meshRef} position={pos} rotation={[0, rotY, 0]}>
      {/* Vehicle Chassis */}
      <mesh position={[0, 0.25, 0]} castShadow>
        <boxGeometry args={[1.8, 0.6, 3.8]} />
        <meshStandardMaterial
          color={isEmergency ? '#e11d48' : '#0284c7'}
          metalness={0.7}
          roughness={0.25}
          emissive={isEmergency ? '#9f1239' : '#0369a1'}
          emissiveIntensity={0.2}
        />
      </mesh>

      {/* Vehicle Cabin / Glass Canopy */}
      <mesh position={[0, 0.72, -0.2]}>
        <boxGeometry args={[1.5, 0.5, 2.0]} />
        <meshStandardMaterial color="#0f172a" roughness={0.1} metalness={0.9} />
      </mesh>

      {/* Headlights (Front, toward -Z in vehicle space) */}
      <mesh position={[0.65, 0.25, -1.9]}>
        <sphereGeometry args={[0.1, 12, 12]} />
        <meshBasicMaterial color="#e0f2fe" />
      </mesh>
      <mesh position={[-0.65, 0.25, -1.9]}>
        <sphereGeometry args={[0.1, 12, 12]} />
        <meshBasicMaterial color="#e0f2fe" />
      </mesh>

      {/* Taillights (Back, toward +Z in vehicle space) */}
      <mesh position={[0.65, 0.25, 1.9]}>
        <sphereGeometry args={[0.1, 12, 12]} />
        <meshBasicMaterial color="#f43f5e" />
      </mesh>
      <mesh position={[-0.65, 0.25, 1.9]}>
        <sphereGeometry args={[0.1, 12, 12]} />
        <meshBasicMaterial color="#f43f5e" />
      </mesh>

      {/* Emergency Siren Light Bar */}
      {isEmergency && (
        <group position={[0, 1.05, -0.2]}>
          <mesh>
            <boxGeometry args={[0.9, 0.16, 0.3]} />
            <meshStandardMaterial color="#f43f5e" emissive="#ff0033" emissiveIntensity={2.5} />
          </mesh>
          <pointLight ref={strobeLightRef} color="#ff0033" intensity={2.5} distance={10} />
        </group>
      )}

      {/* Floating ID Indicator */}
      <Text
        position={[0, 1.6, 0]}
        fontSize={0.35}
        color={isEmergency ? '#ff4d6d' : '#38bdf8'}
        anchorX="center"
        anchorY="middle"
      >
        {isEmergency ? `🚨 #${vehicle.id}` : `#${vehicle.id}`}
      </Text>
    </group>
  )
}

function LaneVehiclesGroup({ lane, tick }: { lane: LaneData; tick: number }) {
  const direction = lane.name as 'North_Bound' | 'South_Bound' | 'East_Bound' | 'West_Bound'

  return (
    <group>
      {lane.vehicles.slice(0, 8).map((vehicle, idx) => (
        <SimulationVehicle
          key={`${lane.name}-${vehicle.id}-${idx}`}
          vehicle={vehicle}
          queueIndex={idx}
          direction={direction}
          signal={lane.signal}
          tick={tick}
        />
      ))}
    </group>
  )
}

// ---------------------------------------------------------------------------
// 3D Scene Assembly
// ---------------------------------------------------------------------------

function TrafficScene({
  lanes,
  tick,
  controlsRef,
}: {
  lanes: LaneData[]
  tick: number
  controlsRef: React.RefObject<OrbitControlsImpl | null>
}) {
  const getSignal = (name: string): 'RED' | 'YELLOW' | 'GREEN' => {
    return lanes.find((l) => l.name === name)?.signal ?? 'RED'
  }

  return (
    <>
      <color attach="background" args={['#060810']} />
      <fog attach="fog" args={['#060810', 45, 110]} />

      {/* Lighting */}
      <ambientLight intensity={0.4} color="#38bdf8" />
      <directionalLight
        position={[25, 45, 25]}
        intensity={1.2}
        color="#f8fafc"
        castShadow
        shadow-mapSize-width={2048}
        shadow-mapSize-height={2048}
      />
      <pointLight position={[0, 18, 0]} intensity={0.6} color="#00d4ff" distance={60} />

      {/* Road Infrastructure & Markings */}
      <RoadIntersection />
      <RoadMarkings />

      {/* 4 Traffic Light Gantries */}
      {/* North Gantry */}
      <TrafficLightGantry
        position={[6.5, 0, 7.0]}
        rotationY={0}
        signal={getSignal('North_Bound')}
        laneName="NORTH"
      />
      {/* South Gantry */}
      <TrafficLightGantry
        position={[-6.5, 0, -7.0]}
        rotationY={Math.PI}
        signal={getSignal('South_Bound')}
        laneName="SOUTH"
      />
      {/* East Gantry */}
      <TrafficLightGantry
        position={[7.0, 0, -6.5]}
        rotationY={Math.PI / 2}
        signal={getSignal('East_Bound')}
        laneName="EAST"
      />
      {/* West Gantry */}
      <TrafficLightGantry
        position={[-7.0, 0, 6.5]}
        rotationY={-Math.PI / 2}
        signal={getSignal('West_Bound')}
        laneName="WEST"
      />

      {/* Live Animated Vehicles */}
      {lanes.map((lane) => (
        <LaneVehiclesGroup key={lane.name} lane={lane} tick={tick} />
      ))}

      {/* OrbitControls with boundaries */}
      <OrbitControls
        ref={controlsRef}
        enablePan={true}
        enableZoom={true}
        enableRotate={true}
        minDistance={8}
        maxDistance={80}
        maxPolarAngle={Math.PI / 2.05}
        target={[0, 1, 0]}
      />
    </>
  )
}

// ---------------------------------------------------------------------------
// Main 3D Simulation Page
// ---------------------------------------------------------------------------

export default function Simulation3D() {
  const { state, connected, fetchState, sendControl, isControlling } = useSimStore()
  const controlsRef = useRef<OrbitControlsImpl | null>(null)
  const [cameraMode, setCameraMode] = useState<'isometric' | 'top' | 'street'>('isometric')

  useEffect(() => {
    fetchState()
    const interval = setInterval(fetchState, 1000)
    return () => clearInterval(interval)
  }, [fetchState])

  // Camera Presets
  const setCameraPreset = (preset: 'isometric' | 'top' | 'street') => {
    if (!controlsRef.current) return
    const controls = controlsRef.current
    setCameraMode(preset)

    if (preset === 'isometric') {
      controls.object.position.set(24, 22, 24)
      controls.target.set(0, 1, 0)
    } else if (preset === 'top') {
      controls.object.position.set(0, 48, 0.1)
      controls.target.set(0, 0, 0)
    } else if (preset === 'street') {
      controls.object.position.set(4, 2.5, 20)
      controls.target.set(0, 2, 0)
    }
    controls.update()
  }

  const resetCamera = () => setCameraPreset('isometric')

  const lanes = state?.lanes ?? []
  const tick = state?.tick ?? 0
  const emergencyActive = lanes.some((l) => l.hasEmergency)

  return (
    <div className="relative w-full h-[calc(100vh-4rem)] bg-[#060810] overflow-hidden select-none">
      {/* 3D WebGL Canvas */}
      <Canvas
        camera={{ position: [24, 22, 24], fov: 48 }}
        shadows
        gl={{ antialias: true }}
      >
        <TrafficScene lanes={lanes} tick={tick} controlsRef={controlsRef} />
      </Canvas>

      {/* HUD: Top-Left Telemetry Box */}
      <div className="absolute top-4 left-4 glass-panel-glow rounded-2xl p-4 min-w-[240px] pointer-events-auto">
        <div className="flex items-center justify-between border-b border-slate-800/80 pb-2 mb-3">
          <div className="flex items-center gap-2">
            <span className="w-2.5 h-2.5 rounded-full bg-cyan-400 animate-ping" />
            <span className="text-xs font-mono font-bold tracking-wider text-white">INTERSECTION 3D</span>
          </div>
          <span className="text-[10px] font-mono text-cyan-400 bg-cyan-500/10 px-2 py-0.5 rounded border border-cyan-500/30">
            SIM #{tick}
          </span>
        </div>

        <div className="space-y-2 text-xs font-mono">
          <div className="flex justify-between text-slate-400">
            <span>Status:</span>
            <span className={`font-semibold uppercase ${connected ? 'text-emerald-400' : 'text-rose-400'}`}>
              {connected ? state?.status ?? 'ONLINE' : 'OFFLINE'}
            </span>
          </div>
          <div className="flex justify-between text-slate-400">
            <span>Vehicles Active:</span>
            <span className="font-bold text-white">
              {lanes.reduce((s, l) => s + l.queueSize, 0)}
            </span>
          </div>
          <div className="flex justify-between text-slate-400">
            <span>Total Cleared:</span>
            <span className="font-bold text-cyan-400">
              {state?.stats.totalProcessed ?? 0}
            </span>
          </div>
          {emergencyActive && (
            <div className="mt-2 pt-2 border-t border-rose-500/40 flex items-center gap-1.5 text-rose-400 font-bold animate-pulse">
              <ShieldAlert className="w-4 h-4" />
              <span>EMERGENCY PREEMPTION</span>
            </div>
          )}
        </div>
      </div>

      {/* HUD: Top-Right Signal Monitor */}
      <div className="absolute top-4 right-4 glass-panel rounded-2xl p-4 min-w-[220px] pointer-events-auto hidden sm:block">
        <div className="text-xs font-mono font-bold tracking-wider text-slate-300 uppercase mb-3 flex items-center justify-between">
          <span>Signal Telemetry</span>
          <Compass className="w-4 h-4 text-cyan-400" />
        </div>
        <div className="space-y-2">
          {lanes.map((lane) => {
            const isGreen = lane.signal === 'GREEN'
            const isYellow = lane.signal === 'YELLOW'
            return (
              <div
                key={lane.name}
                className="flex items-center justify-between text-xs font-mono p-1.5 rounded-lg bg-slate-900/60 border border-slate-800"
              >
                <span className="text-slate-300 font-medium">
                  {lane.name.replace('_Bound', '')}
                </span>
                <div className="flex items-center gap-2">
                  <span className="text-slate-500">({lane.queueSize})</span>
                  <span
                    className={`px-2 py-0.5 rounded text-[10px] font-bold ${
                      isGreen
                        ? 'bg-emerald-500/20 text-emerald-400 border border-emerald-500/40'
                        : isYellow
                        ? 'bg-amber-500/20 text-amber-400 border border-amber-500/40'
                        : 'bg-rose-500/20 text-rose-400 border border-rose-500/40'
                    }`}
                  >
                    {lane.signal}
                  </span>
                </div>
              </div>
            )
          })}
        </div>
      </div>

      {/* Floating Bottom Control Bar */}
      <div className="absolute bottom-6 left-1/2 -translate-x-1/2 glass-panel-glow rounded-2xl px-6 py-3 flex flex-wrap items-center gap-4 pointer-events-auto border border-cyan-500/30">
        {/* Simulation Controls */}
        <div className="flex items-center gap-2 border-r border-slate-800 pr-4">
          {state?.status === 'running' ? (
            <button
              onClick={() => sendControl('pause')}
              disabled={isControlling}
              className="flex items-center gap-1.5 px-3.5 py-1.5 rounded-xl text-xs font-semibold bg-amber-500/20 border border-amber-500/40 text-amber-300 hover:bg-amber-500/30 transition-all shadow-[0_0_12px_rgba(245,158,11,0.25)]"
            >
              <Pause className="w-3.5 h-3.5" />
              <span>Pause</span>
            </button>
          ) : (
            <button
              onClick={() => sendControl('resume')}
              disabled={isControlling}
              className="flex items-center gap-1.5 px-3.5 py-1.5 rounded-xl text-xs font-semibold bg-emerald-500/20 border border-emerald-500/40 text-emerald-300 hover:bg-emerald-500/30 transition-all shadow-[0_0_12px_rgba(16,185,129,0.3)]"
            >
              <Play className="w-3.5 h-3.5" />
              <span>{state?.status === 'paused' ? 'Resume' : 'Start'}</span>
            </button>
          )}

          <button
            onClick={() => sendControl('reset')}
            disabled={isControlling}
            title="Reset Simulation Engine"
            className="flex items-center gap-1.5 px-3 py-1.5 rounded-xl text-xs font-semibold bg-slate-800 border border-slate-700 text-slate-300 hover:text-white hover:bg-slate-700 transition-all"
          >
            <RotateCcw className="w-3.5 h-3.5" />
            <span>Reset</span>
          </button>
        </div>

        {/* Camera Preset Buttons */}
        <div className="flex items-center gap-2">
          <span className="text-[11px] font-mono text-slate-400 uppercase hidden sm:inline">Camera:</span>
          <button
            onClick={() => setCameraPreset('isometric')}
            className={`px-3 py-1.5 rounded-lg text-xs font-medium transition-all ${
              cameraMode === 'isometric'
                ? 'bg-cyan-500/20 text-cyan-300 border border-cyan-500/40 font-semibold'
                : 'text-slate-400 hover:text-white bg-slate-900 border border-slate-800'
            }`}
          >
            Isometric
          </button>
          <button
            onClick={() => setCameraPreset('top')}
            className={`px-3 py-1.5 rounded-lg text-xs font-medium transition-all ${
              cameraMode === 'top'
                ? 'bg-cyan-500/20 text-cyan-300 border border-cyan-500/40 font-semibold'
                : 'text-slate-400 hover:text-white bg-slate-900 border border-slate-800'
            }`}
          >
            Aerial
          </button>
          <button
            onClick={() => setCameraPreset('street')}
            className={`px-3 py-1.5 rounded-lg text-xs font-medium transition-all ${
              cameraMode === 'street'
                ? 'bg-cyan-500/20 text-cyan-300 border border-cyan-500/40 font-semibold'
                : 'text-slate-400 hover:text-white bg-slate-900 border border-slate-800'
            }`}
          >
            Street View
          </button>

          <button
            onClick={resetCamera}
            title="Reset Camera Target and Angle"
            className="p-1.5 rounded-lg bg-slate-900 border border-slate-800 text-slate-400 hover:text-white hover:border-slate-700 transition-all ml-1"
          >
            <Maximize2 className="w-4 h-4" />
          </button>
        </div>
      </div>
    </div>
  )
}
