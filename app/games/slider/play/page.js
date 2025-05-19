"use client"
import Link from 'next/link'
import NumberSlider from "@/components/number-slider"
import { Card } from "@/components/ui/card"
import { ThemeToggle } from "@/components/theme-toggle"

export default function SliderPlayPage() {
  return (
    <main className="container mx-auto px-4 py-8">
      <div className="flex justify-between items-center mb-8">
        <Link href="/games/slider" className="text-muted-foreground hover:text-foreground transition-colors">
          ← Back to Number Slider
        </Link>
        <ThemeToggle />
      </div>

      <Card>
          <NumberSlider />
      </Card>
    </main>
  )
}
