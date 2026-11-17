import { BrowserRouter, Routes, Route } from 'react-router-dom'
import NavBar from './components/NavBar'
import Dashboard from './pages/Dashboard'
import Simulation3D from './pages/Simulation3D'

export default function App() {
  return (
    <BrowserRouter>
      <div className="min-h-screen bg-[#060810] text-slate-100 flex flex-col font-sans">
        <NavBar />
        <main className="pt-16 flex-1">
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/simulation" element={<Simulation3D />} />
          </Routes>
        </main>
      </div>
    </BrowserRouter>
  )
}
