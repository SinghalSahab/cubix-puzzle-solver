"use client"
import Link from 'next/link'
import Sudoku from "@/components/sudoku"
import { Card } from "@/components/ui/card"
import { ThemeToggle } from "@/components/theme-toggle"

export default function SudokuPlayPage() {
  return (
    <main className="container mx-auto px-4 py-8">
      <div className="flex justify-between items-center mb-8">
        <Link href="/games/sudoku" className="text-muted-foreground hover:text-foreground transition-colors">
          ← Back to Sudoku
        </Link>
        <ThemeToggle />
      </div>

      <Card>
          <Sudoku />
      </Card>
    </main>
  )
}
