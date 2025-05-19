"use client"
import Link from 'next/link'
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Grid3x3, Play } from "lucide-react"
import { ThemeToggle } from "@/components/theme-toggle"
import Image from "next/image"

export default function RbkPage() {
  return (
    <main className="container mx-auto px-4 py-8">
      <div className="flex justify-between items-center mb-8">
        <Link href="/" className="text-muted-foreground hover:text-foreground transition-colors">
          ← Back to Games
        </Link>
        <ThemeToggle />
      </div>

      <Card className="mb-8 bg-card text-card-foreground shadow-lg">
        <CardHeader>
          <div className="flex items-center gap-4">
            <div className="p-3 rounded-xl bg-gradient-to-br from-red-500 to-blue-500 shadow-md">
              <Grid3x3 className="h-6 w-6 text-white" />
            </div>
            <div>
              <CardTitle className="text-2xl md:text-3xl text-primary">Rubik's Cube</CardTitle>
              <CardDescription className="text-muted-foreground">Master the classic 3D combination puzzle</CardDescription>
            </div>
          </div>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
            <div className="prose dark:prose-invert text-foreground">
              <h2 className="text-2xl font-semibold text-primary">How to Play</h2>
              <p className="text-gray-700 dark:text-gray-300">
                The Rubik's Cube is a 3D combination puzzle invented in 1974. The goal is to return each face to a solid color.
              </p>
              <h3 className="text-xl font-medium mt-4">Rules:</h3>
              <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
                <li>Each face can be rotated clockwise or counterclockwise</li>
                <li>The cube is solved when all sides show a single color</li>
                <li>A standard 3x3x3 cube has over 43 quintillion possible combinations</li>
              </ul>
              <h3 className="text-xl font-medium mt-4">Controls:</h3>
              <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
                <li>Click and drag to rotate the cube view</li>
                <li>Click face buttons to rotate cube faces</li>
                <li>Use keyboard shortcuts (F, B, R, L, U, D) for faster moves</li>
              </ul>
            </div>
            <div className="w-full max-w-sm mx-auto">
              <Image
                className="rounded-lg shadow-lg"
                src="https://www.picgifs.com/graphics/r/rubiks-cube/animaatjes-rubiks-cube-7217468.gif"
                alt="Rubik's Cube Animation"
                width={400}
                height={400}
              />
            </div>
          </div>
        </CardContent>
      </Card>

      <div className="grid grid-cols-1 sm:grid-cols-1 gap-4 md:gap-6">
        <Link href="/games/rubiks/play" className="transition-transform hover:scale-[1.02] active:scale-[0.98]">
          <Button className="w-full h-24 md:h-32 text-lg md:text-xl gap-4 bg-primary text-primary-foreground hover:bg-primary/90">
            <Play className="h-6 w-6" />
            Play Game
          </Button>
        </Link>
      </div>
    </main>
  )
}
