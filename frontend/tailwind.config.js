/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        cyber: {
          bg: '#060810',
          base: '#0a0e1a',
          panel: '#0d1424',
          subpanel: '#131c31',
          border: 'rgba(29, 111, 232, 0.25)',
          borderglow: 'rgba(0, 212, 255, 0.4)',
          cyan: '#00d4ff',
          blue: '#1d6fe8',
          accent: '#38bdf8',
          emergency: '#ef4444'
        }
      },
      fontFamily: {
        sans: ['Inter', 'system-ui', 'sans-serif'],
        mono: ['JetBrains Mono', 'Fira Code', 'monospace'],
      },
      boxShadow: {
        'glow-cyan': '0 0 25px rgba(0, 212, 255, 0.25)',
        'glow-blue': '0 0 25px rgba(29, 111, 232, 0.25)',
        'glow-red': '0 0 25px rgba(239, 68, 68, 0.35)',
      }
    },
  },
  plugins: [],
}
