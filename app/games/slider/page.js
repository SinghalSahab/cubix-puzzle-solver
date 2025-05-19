"use client"
import Link from 'next/link'
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Grid2x2, Play } from "lucide-react"
import { ThemeToggle } from "@/components/theme-toggle"

export default function SldrPage() {
  return (
    <main className="container mx-auto px-4 py-8">
      <div className="flex justify-between items-center mb-8">
        <Link href="/" className="text-muted-foreground hover:text-foreground transition-colors">
          ← Back to Games
        </Link>
        <ThemeToggle />
      </div>

      <Card className="mb-8">
        <CardHeader>
          <div className="flex items-center gap-4">
            <div className="p-3 rounded-xl bg-[url('https://www.shutterstock.com/image-vector/uniform-walnut-wooden-texture-horizontal-600nw-2221081683.jpg')]">
              <Grid2x2 className="h-6 w-6 text-white" />
            </div>
            <div>
              <CardTitle className="text-3xl text-[#4F362As]">Number Slider</CardTitle>
              <CardDescription>Arrange the numbers in ascending order</CardDescription>
            </div>
          </div>
        </CardHeader>
        <CardContent className="prose dark:prose-invert">
          <h2 className="text-2xl font-semibold text-[#4F362A]">How to Play</h2>
          <p className="text-gray-700 dark:text-gray-300">
            The Number Slider puzzle consists of a grid of numbered tiles with one empty space. The goal is to arrange the numbers in ascending order.
          </p>
          <h3 className="text-xl font-medium mt-4">Rules:</h3>
          <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
            <li>Click any tile adjacent to the empty space to move it</li>
            <li>Numbers must be arranged in ascending order from left to right, top to bottom</li>
            <li>The empty space should end up in the bottom-right corner</li>
          </ul>
          <h3 className="text-xl font-medium mt-4">Controls:</h3>
          <ul className="list-disc list-inside space-y-2 text-gray-700 dark:text-gray-300">
            <li>Click tiles to move them into the empty space</li>
            <li>Use arrow keys to move tiles (optional)</li>
          </ul>
        </CardContent>
      </Card>

      <div className="grid grid-cols-1 gap-6">
        <Link href="/games/slider/play">
          <Button 
            className="w-full h-32 text-xl gap-4 text-white" 
            variant="default"
            style={{
              backgroundImage: "url('https://www.shutterstock.com/image-vector/uniform-walnut-wooden-texture-horizontal-600nw-2221081683.jpg')",
              backgroundSize: "cover",
              backgroundRepeat: "no-repeat",
              backgroundPosition: "center"
            }}
          >
            <Play className="h-6 w-6" />
            Play Game
          </Button>
        </Link>
      </div>
    </main>
  )
}
