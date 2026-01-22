import { useState } from 'react'

// Icons as simple SVG components
const FolderIcon = () => (
  <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2}
          d="M3 7v10a2 2 0 002 2h14a2 2 0 002-2V9a2 2 0 00-2-2h-6l-2-2H5a2 2 0 00-2 2z" />
  </svg>
)

const SpeedIcon = () => (
  <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2}
          d="M13 10V3L4 14h7v7l9-11h-7z" />
  </svg>
)

const ShieldIcon = () => (
  <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2}
          d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
  </svg>
)

const CodeIcon = () => (
  <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2}
          d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4" />
  </svg>
)

const GithubIcon = () => (
  <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 24 24">
    <path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"/>
  </svg>
)

const DownloadIcon = () => (
  <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2}
          d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
  </svg>
)

const features = [
  {
    icon: <FolderIcon />,
    title: 'Smart Categorization',
    description: 'Automatically sorts files into 11 categories based on file types: Images, Documents, Videos, Audio, Code, and more.'
  },
  {
    icon: <SpeedIcon />,
    title: 'Parallel Processing',
    description: 'Multi-threaded architecture processes thousands of files in seconds using modern C++20 parallelization.'
  },
  {
    icon: <ShieldIcon />,
    title: 'Safe & Reversible',
    description: 'Dry-run mode previews changes. Complete operation logs enable full undo capability. Your files are always safe.'
  },
  {
    icon: <CodeIcon />,
    title: 'Open Source',
    description: 'MIT licensed, fully open source. Inspect, modify, and contribute. Built with modern C++20 standards.'
  }
]

const categories = [
  { name: 'Images', extensions: '.jpg .png .gif .svg .webp', color: 'bg-pink-500' },
  { name: 'Documents', extensions: '.pdf .doc .docx .txt .md', color: 'bg-blue-500' },
  { name: 'Videos', extensions: '.mp4 .mkv .avi .mov .webm', color: 'bg-purple-500' },
  { name: 'Audio', extensions: '.mp3 .wav .flac .ogg .m4a', color: 'bg-green-500' },
  { name: 'Code', extensions: '.cpp .py .js .ts .java .rs', color: 'bg-yellow-500' },
  { name: 'Archives', extensions: '.zip .rar .7z .tar .gz', color: 'bg-red-500' },
]

function App() {
  const [showDemo, setShowDemo] = useState(false)

  return (
    <div className="min-h-screen">
      {/* Hero Section */}
      <section className="relative overflow-hidden">
        {/* Background effects */}
        <div className="absolute inset-0 bg-gradient-to-br from-steel-900 via-steel-800 to-steel-900" />
        <div className="absolute inset-0 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNjAiIGhlaWdodD0iNjAiIHZpZXdCb3g9IjAgMCA2MCA2MCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZyBmaWxsPSJub25lIiBmaWxsLXJ1bGU9ImV2ZW5vZGQiPjxnIGZpbGw9IiMyMDI5M2IiIGZpbGwtb3BhY2l0eT0iMC40Ij48cGF0aCBkPSJNMzYgMzRoLTJ2LTRoMnY0em0wLThoLTJ2LTRoMnY0em0tOCA4aC0ydi00aDJ2NHptMC04aC0ydi00aDJ2NHoiLz48L2c+PC9nPjwvc3ZnPg==')] opacity-30" />

        <div className="relative max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 pt-20 pb-32">
          <nav className="flex items-center justify-between mb-20">
            <div className="flex items-center gap-3">
              <div className="w-10 h-10 bg-forge-500 rounded-lg flex items-center justify-center animate-glow">
                <span className="text-xl font-bold text-white">F</span>
              </div>
              <span className="text-2xl font-bold">FileForge</span>
            </div>
            <div className="flex items-center gap-6">
              <a href="#features" className="text-steel-300 hover:text-white transition-colors">Features</a>
              <a href="#download" className="text-steel-300 hover:text-white transition-colors">Download</a>
              <a href="https://github.com/CrAvila/SO" target="_blank" rel="noopener noreferrer"
                 className="flex items-center gap-2 text-steel-300 hover:text-white transition-colors">
                <GithubIcon />
                <span className="hidden sm:inline">GitHub</span>
              </a>
            </div>
          </nav>

          <div className="text-center max-w-4xl mx-auto">
            <h1 className="text-5xl sm:text-6xl lg:text-7xl font-extrabold mb-6 animate-fade-in-up">
              <span className="gradient-text">Forging Order</span>
              <br />
              <span className="text-white">from Chaos</span>
            </h1>

            <p className="text-xl sm:text-2xl text-steel-300 mb-10 animate-fade-in-up animate-delay-100">
              Transform cluttered folders into perfectly organized collections.
              <br className="hidden sm:block" />
              High-performance file organization for Windows, macOS, and Linux.
            </p>

            <div className="flex flex-col sm:flex-row gap-4 justify-center animate-fade-in-up animate-delay-200">
              <a href="#download" className="forge-button flex items-center justify-center gap-2">
                <DownloadIcon />
                Download Now
              </a>
              <button
                onClick={() => setShowDemo(!showDemo)}
                className="forge-button-outline"
              >
                See How It Works
              </button>
            </div>
          </div>

          {/* Demo Terminal */}
          {showDemo && (
            <div className="mt-16 max-w-3xl mx-auto animate-fade-in-up">
              <div className="bg-steel-800 rounded-xl border border-steel-700 overflow-hidden shadow-2xl">
                <div className="flex items-center gap-2 px-4 py-3 bg-steel-900 border-b border-steel-700">
                  <div className="w-3 h-3 rounded-full bg-red-500" />
                  <div className="w-3 h-3 rounded-full bg-yellow-500" />
                  <div className="w-3 h-3 rounded-full bg-green-500" />
                  <span className="ml-4 text-steel-400 text-sm font-mono">Terminal</span>
                </div>
                <div className="p-6 font-mono text-sm">
                  <div className="text-steel-400">$ fileforge ~/Downloads</div>
                  <div className="mt-4 text-forge-400">
                    Scanning directory: /home/user/Downloads
                  </div>
                  <div className="text-steel-300">Found 847 files</div>
                  <div className="mt-2 text-steel-300">
                    [========================================] 100% (847/847)
                  </div>
                  <div className="mt-4 text-green-400">Organization Complete!</div>
                  <div className="text-steel-300">
                    --------------------------------------------------<br />
                    Total files:     847<br />
                    Processed:       <span className="text-green-400">847</span><br />
                    Moved:           847<br />
                    Renamed:         623<br />
                    Duration:        1,234 ms<br />
                    Success rate:    100.0%<br />
                    --------------------------------------------------
                  </div>
                </div>
              </div>
            </div>
          )}
        </div>
      </section>

      {/* Features Section */}
      <section id="features" className="py-24 bg-steel-800/30">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="text-center mb-16">
            <h2 className="text-3xl sm:text-4xl font-bold mb-4">
              Powerful Features
            </h2>
            <p className="text-steel-400 text-lg max-w-2xl mx-auto">
              Built with modern C++20 for maximum performance and reliability
            </p>
          </div>

          <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-6">
            {features.map((feature, index) => (
              <div key={index} className="feature-card">
                <div className="w-12 h-12 bg-forge-500/20 rounded-lg flex items-center justify-center text-forge-400 mb-4">
                  {feature.icon}
                </div>
                <h3 className="text-xl font-semibold mb-2">{feature.title}</h3>
                <p className="text-steel-400">{feature.description}</p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Categories Section */}
      <section className="py-24">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="text-center mb-16">
            <h2 className="text-3xl sm:text-4xl font-bold mb-4">
              Intelligent Categorization
            </h2>
            <p className="text-steel-400 text-lg max-w-2xl mx-auto">
              Files are automatically sorted into intuitive categories based on their type
            </p>
          </div>

          <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-6 gap-4">
            {categories.map((cat, index) => (
              <div key={index} className="bg-steel-800/50 border border-steel-700 rounded-xl p-4 text-center hover:border-forge-500/50 transition-colors">
                <div className={`w-4 h-4 ${cat.color} rounded-full mx-auto mb-3`} />
                <h3 className="font-semibold mb-2">{cat.name}</h3>
                <p className="text-xs text-steel-500 font-mono">{cat.extensions}</p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Download Section */}
      <section id="download" className="py-24 bg-steel-800/30">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <h2 className="text-3xl sm:text-4xl font-bold mb-4">
            Get Started
          </h2>
          <p className="text-steel-400 text-lg mb-10">
            Build from source or download pre-built binaries
          </p>

          <div className="code-block text-left mb-10">
            <div className="text-steel-400 mb-2"># Clone and build</div>
            <div className="text-steel-200">
              git clone https://github.com/CrAvila/SO.git<br />
              cd SO<br />
              mkdir build && cd build<br />
              cmake .. -DCMAKE_BUILD_TYPE=Release<br />
              cmake --build . --parallel
            </div>
          </div>

          <div className="flex flex-col sm:flex-row gap-4 justify-center">
            <a href="https://github.com/CrAvila/SO/releases"
               className="forge-button flex items-center justify-center gap-2">
              <DownloadIcon />
              Download Latest Release
            </a>
            <a href="https://github.com/CrAvila/SO"
               target="_blank"
               rel="noopener noreferrer"
               className="forge-button-outline flex items-center justify-center gap-2">
              <GithubIcon />
              View on GitHub
            </a>
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="py-12 border-t border-steel-800">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex flex-col md:flex-row items-center justify-between gap-6">
            <div className="flex items-center gap-3">
              <div className="w-8 h-8 bg-forge-500 rounded-lg flex items-center justify-center">
                <span className="text-sm font-bold text-white">F</span>
              </div>
              <span className="text-lg font-bold">FileForge</span>
            </div>

            <p className="text-steel-500 text-sm">
              Originally conceived as an Operating Systems project at UPAEP.
              <br className="sm:hidden" />
              <span className="hidden sm:inline"> | </span>
              Open source under MIT License.
            </p>

            <div className="flex items-center gap-6">
              <a href="https://github.com/CrAvila/SO"
                 target="_blank"
                 rel="noopener noreferrer"
                 className="text-steel-400 hover:text-white transition-colors">
                <GithubIcon />
              </a>
            </div>
          </div>
        </div>
      </footer>
    </div>
  )
}

export default App
