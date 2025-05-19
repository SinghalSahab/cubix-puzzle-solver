"use client"
import Link from 'next/link'
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Grid3x3, Wand2 } from "lucide-react"
import { ThemeToggle } from "@/components/theme-toggle"

export default function SudokuPage() {
  return (
    <main className="container mx-auto px-4 py-8">
      <div className="flex justify-between items-center mb-8">
        <Link href="/" className="text-muted-foreground hover:text-foreground transition-colors text-lg font-medium">
          ← Back to Games
        </Link>
        <ThemeToggle />
      </div>

      <Card className="mb-8 shadow-lg border border-gray-200 dark:border-gray-700">
        <CardHeader>
          <div className="flex items-center gap-4">
            <div className="p-3 rounded-xl bg-gradient-to-r from-purple-500 to-indigo-500 shadow-md">
              <Grid3x3 className="h-8 w-8 text-white" />
            </div>
            <div>
              <CardTitle className="text-4xl font-bold">Sudoku</CardTitle>
              <CardDescription className="text-lg text-gray-600 dark:text-gray-400">
                Fill the grid with numbers following Sudoku rules
              </CardDescription>
            </div>
          </div>
        </CardHeader>
        <CardContent className="prose dark:prose-invert">
          <h2 className="text-2xl font-semibold text-[#7F38E8]">How to Play</h2>
          <p className="text-gray-700 dark:text-gray-300">
            Sudoku is a logic-based number placement puzzle. The goal is to fill a 9×9 grid with digits so that each column, each row, and each of the nine 3×3 subgrids contain all of the digits from 1 to 9.
          </p>
          <h3 className="text-xl font-medium mt-4">Rules:</h3>
          <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
            <li>Fill each empty cell with a number from 1 to 9</li>
            <li>Each row must contain all numbers from 1 to 9</li>
            <li>Each column must contain all numbers from 1 to 9</li>
            <li>Each 3x3 box must contain all numbers from 1 to 9</li>
          </ul>
          <h3 className="text-xl font-medium mt-4">Controls:</h3>
          <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
            <li>Click a cell to select it</li>
            <li>Type a number (1-9) to fill the selected cell</li>
            <li>Press Delete or Backspace to clear a cell</li>
          </ul>
        </CardContent>
      </Card>

      <div className="grid grid-cols-1 md:grid-cols-1 gap-6">
        <Link href="/games/sudoku/play">
          <Button className="w-full h-32 text-xl gap-4 bg-gradient-to-r from-purple-500 to-indigo-500 text-white hover:from-purple-600 hover:to-indigo-600 shadow-lg">
            <Wand2 className="h-8 w-8" />
            Play and Solve Game
          </Button>
        </Link>
      </div>
    </main>
  )
}
