"use client";

import Link from "next/link";
import { Trophy, Brain, Clock, Sparkles, Grid3x3, Grid2x2 } from "lucide-react";
import { Card, CardContent } from "@/components/ui/card";
import { ThemeToggle } from "@/components/theme-toggle";

export default function Home() {
  return (
    <main className="min-h-screen bg-background text-foreground">
      <div className="container mx-auto px-4 py-8">
        <div className="flex justify-end mb-4">
          <ThemeToggle />
        </div>

        <header className="mb-12 text-center animate-slide-in">
          <div className="inline-flex items-center gap-2 mb-4">
            <Sparkles className="h-8 w-8 text-primary animate-pulse-slow" />
            <h1 className="text-4xl md:text-5xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-primary to-primary/80">
              Cubix
            </h1>
          </div>
          <p className="text-lg md:text-xl text-muted-foreground mb-8">
            Master Algorithmic Puzzle Solving
          </p>
        </header>

        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 md:gap-6">
          <Link
            href="/games/rubiks"
            className="transition-transform hover:scale-[1.02] active:scale-[0.98]"
          >
            <Card className="hover:bg-gradient-to-br from-red-500 to-yellow-500 transition-colors cursor-pointer h-full">
              <CardContent className="flex flex-col items-center gap-4 p-6">
                <div className="w-16 h-16 flex items-center justify-center bg-gradient-to-br from-red-500 to-blue-500 rounded-full shadow-lg">
                  <Grid3x3 className="h-8 w-8 text-white" />
                </div>
                <h2 className="text-xl font-semibold">Rubik's Cube</h2>
              </CardContent>
            </Card>
          </Link>

          <Link
            href="/games/slider"
            className="transition-transform hover:scale-[1.02] active:scale-[0.98]"
          >
            <Card className="hover:bg-amber-800/50 transition-colors cursor-pointer h-full">
              <CardContent className="flex flex-col items-center gap-4 p-6">
                <div className="p-3 rounded-full bg-[url('https://www.shutterstock.com/image-vector/uniform-walnut-wooden-texture-horizontal-600nw-2221081683.jpg')]">
                  <Grid2x2 className="h-6 w-6 text-white" />
                </div>

                <h2 className="text-xl font-semibold">Number Slider</h2>
              </CardContent>
            </Card>
          </Link>

          <Link
            href="/games/sudoku"
            className="transition-transform hover:scale-[1.02] active:scale-[0.98]"
          >
            <Card className="hover:bg-purple-400 transition-colors cursor-pointer h-full">
              <CardContent className="flex flex-col items-center gap-4 p-6">
                <div className="p-3 rounded-full bg-gradient-to-r from-purple-500 to-indigo-500 shadow-md">
                  <Grid3x3 className="h-8 w-8 text-white" />
                </div>
                <h2 className="text-xl font-semibold">Sudoku</h2>
              </CardContent>
            </Card>
          </Link>
        </div>
      </div>
    </main>
  );
}
