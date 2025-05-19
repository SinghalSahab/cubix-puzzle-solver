import { Inter } from "next/font/google"
import { ThemeProvider } from "@/components/theme-provider"
import { cn } from "@/lib/utils"
import "./globals.css"
import WasmInitializer from "@/components/wasm-initializer"

const inter = Inter({ subsets: ["latin"] })

export const metadata = {
  title: "Cubix - Puzzle Solver",
  description: "Solve Rubik's Cube, Number Slider, and Sudoku puzzles with Cubix",
}

export default function RootLayout({ children }) {
  return (
    <html lang="en" suppressHydrationWarning>
      <body className={cn(inter.className, "min-h-screen bg-background font-sans antialiased theme-transition")}>
        <ThemeProvider attribute="class" defaultTheme="system" enableSystem>
          <WasmInitializer />
          {children}
        </ThemeProvider>
      </body>
    </html>
  )
}

