"use client"

import { useState, useEffect } from "react"
import { Button } from "@/components/ui/button"
import { Sparkles, RotateCcw, Trophy, Lightbulb } from "lucide-react"
import { SudokuService } from "@/lib/services/sudoku-service"

const EMPTY_BOARD = Array(9).fill().map(() => Array(9).fill(''))

export default function Sudoku() {
  const [isSolving, setIsSolving] = useState(false)
  const [board, setBoard] = useState(EMPTY_BOARD)
  const [selectedCell, setSelectedCell] = useState(null)
  const [_, setError] = useState(null)
  const [_1, setMessage] = useState(null)
  const [timeElapsed, setTimeElapsed] = useState(0)
  const [points, setPoints] = useState(100)

  useEffect(() => {
    const initEngine = async () => {
      try {
        await SudokuService.compileSolver()
        fetchNewPuzzle()
      } catch (err) {
          console.error(err);
      }
    }
    initEngine()
  }, [])

  const boardToArray = () => {
    return board.flat().map(cell => cell === '' ? '0' : cell);
  }

  const fetchNewPuzzle = async () => {
    try {
      const response = await fetch('https://sudoku-api.vercel.app/api/dosuku')
      const data = await response.json()
      const puzzle = data.newboard.grids[0]
      const newBoard = puzzle.value.map(row => row.map(cell => cell === 0 ? "" : cell.toString()))
      setBoard(newBoard)
      setMessage('New puzzle loaded!')
      setError(null)
      setPoints(100)
      setTimeElapsed(0)
    } catch (err) {
     console.error(err)
    }
  }

  const handleCellClick = (row, col) => {
    setSelectedCell({ row, col })
    setError(null)
  }

  const handleKeyPress = async (event) => {
    if (!selectedCell) return
    
    const { row, col } = selectedCell
    const key = event.key

    if (key === 'Backspace' || key === 'Delete') {
      const newBoard = board.map(row => [...row])
      newBoard[row][col] = ''
      setBoard(newBoard)
      return
    }

    if (key >= '1' && key <= '9') {
      const newBoard = board.map(row => [...row])
      newBoard[row][col] = key
      setBoard(newBoard)
      
      try {
        const boardArray = newBoard.flat().map(cell => cell === '' ? '0' : cell);
        const result = await SudokuService.checkPuzzle(boardArray);
        const isValid = result.output === 'Valid';
        
        if (!isValid) {
          setPoints(prev => Math.max(0, prev - 10))
         
        } else {
          // Check if solved
          if (!boardArray.includes('0')) {
          }
        }
      } catch (err) {
        console.error('Error checking move:', err)
      }
    }
  }

  const handleSolve = async () => {
    try {
      setIsSolving(true)
      setError(null)
      const boardArray = boardToArray();
      const result = await SudokuService.solvePuzzle(boardArray);
      
      // Parse the result
      const solutionArray = result.output.split(',');
      const newBoard = Array(9).fill().map((_, i) => 
        solutionArray.slice(i * 9, (i + 1) * 9).map(n => n === '0' ? '' : n)
      )
      setBoard(newBoard)
      
    } catch (err) {
      console.error(err)
    } finally {
      setIsSolving(false)
    }
  }

  const handleCheck = async () => {
    try {
      const boardArray = boardToArray();
      const result = await SudokuService.checkPuzzle(boardArray);
      const isValid = result.output === 'Valid';
      
    } catch (err) {
      console.error(err)    }
  }

  const handleHint = async () => {
    if (!selectedCell) {
      return
    }

    try {
      const { row, col } = selectedCell;
      const boardArray = boardToArray();
      const args = [...boardArray, row, col];
      
      const result = await SudokuService.getHint(boardArray, row, col);
      const hint = parseInt(result.output);
      
      if (hint && !isNaN(hint)) {
        const newBoard = board.map(row => [...row])
        newBoard[selectedCell.row][selectedCell.col] = hint.toString()
        setBoard(newBoard)
        setPoints(prev => Math.max(0, prev - 5))
      }
    } catch (err) {
      console.error('Error in handleHint:', err)
    }
  }

  const handleClear = async () => {
    if (!selectedCell) {
      return
    }

    const { row, col } = selectedCell
    const newBoard = board.map(row => [...row])
    newBoard[row][col] = ''
    setBoard(newBoard)
  }

  const handleNumberClick = async (number) => {
    if (!selectedCell) return
    const { row, col } = selectedCell
    const newBoard = board.map(row => [...row])
    newBoard[row][col] = number.toString()
    setBoard(newBoard)
  }

  useEffect(() => {
    const timer = setInterval(() => {
      setTimeElapsed(prev => prev + 1)
    }, 1000)
    return () => clearInterval(timer)
  }, [])

  useEffect(() => {
    window.addEventListener('keydown', handleKeyPress)
    return () => window.removeEventListener('keydown', handleKeyPress)
  }, [selectedCell, board])

  return (
    <>
      <div className="flex flex-col sm:flex-row items-center sm:items-start w-full max-w-full p-4 sm:p-8 bg-gradient-to-br from-blue-50 via-purple-100 to-pink-50 dark:from-gray-800 dark:via-gray-900 dark:to-black text-foreground shadow-2xl rounded-lg border border-purple-300 dark:border-gray-700">
        <div
          className="grid grid-cols-9 border-4 border-yellow-400 dark:border-yellow-600 rounded-lg overflow-hidden"
          style={{ width: '100%', maxWidth: '450px', gap: '2px' }}
        >
          {board.map((row, i) =>
            row.map((cell, j) => {
              const boxRow = Math.floor(i / 3)
              const boxCol = Math.floor(j / 3)
              const isEvenBox = (boxRow + boxCol) % 2 === 0

              return (
                <div
                  key={`${i}-${j}`}
                  className={`
                    flex items-center justify-center
                    text-sm sm:text-lg font-semibold
                    ${selectedCell?.row === i && selectedCell?.col === j ? 'ring-4 ring-yellow-500 bg-yellow-300 dark:bg-yellow-700 z-20' : 'z-10'}
                    ${isEvenBox ? 'bg-blue-300 dark:bg-blue-800' : 'bg-pink-300 dark:bg-pink-800'}
                    hover:bg-yellow-400 dark:hover:bg-yellow-600 hover:z-30 cursor-pointer transition-all
                  `}
                  onClick={() => handleCellClick(i, j)}
                  style={{ aspectRatio: '1', width: '100%' }}
                >
                  {cell}
                </div>
              )
            })
          )}
        </div>
        <div className="flex flex-col gap-4 mt-4 sm:mt-0 sm:ml-8 w-full max-w-sm">
          <div className="flex flex-wrap gap-2">
            <Button
              variant="outline"
              onClick={fetchNewPuzzle}
              className="border-yellow-400 dark:border-yellow-600 text-yellow-600 dark:text-yellow-400 hover:bg-yellow-200 dark:hover:bg-yellow-700"
              style={{ width: '120px', height: '40px' }}
            >
              <RotateCcw className="h-4 w-4 mr-2" />
              New Puzzle
            </Button>
            <Button
              variant="outline"
              onClick={handleCheck}
              className="border-green-400 dark:border-green-600 text-green-600 dark:text-green-400 hover:bg-green-200 dark:hover:bg-green-700"
              style={{ width: '120px', height: '40px' }}
            >
              <Trophy className="h-4 w-4 mr-2" />
              Check
            </Button>
            <Button
              variant="outline"
              onClick={handleHint}
              className="border-blue-400 dark:border-blue-600 text-blue-600 dark:text-blue-400 hover:bg-blue-200 dark:hover:bg-blue-700"
              style={{ width: '120px', height: '40px' }}
            >
              <Lightbulb className="h-4 w-4 mr-2" />
              Hint
            </Button>
            <Button
              variant="outline"
              onClick={handleClear}
              className="border-red-400 dark:border-red-600 text-red-600 dark:text-red-400 hover:bg-red-200 dark:hover:bg-red-700"
              style={{ width: '120px', height: '40px' }}
            >
              <RotateCcw className="h-4 w-4 mr-2" />
              Clear
            </Button>
          </div>
          <span className="text-lg font-semibold text-yellow-600 dark:text-yellow-400">
            Time: {Math.floor(timeElapsed / 60)}:{(timeElapsed % 60).toString().padStart(2, '0')}
          </span>
          <Button
            onClick={handleSolve}
            disabled={isSolving}
            className={`bg-purple-600 dark:bg-purple-800 text-white hover:bg-purple-700 dark:hover:bg-purple-900 ${isSolving ? 'opacity-50' : ''}`}
            style={{ width: '120px', height: '40px' }}
          >
            <Sparkles className="h-4 w-4 mr-2" />
            {isSolving ? 'Solving...' : 'Solve'}
          </Button>
          <div className="grid grid-cols-3 gap-2">
            {[1, 2, 3, 4, 5, 6, 7, 8, 9].map((num) => (
              <Button
                key={num}
                onClick={() => handleNumberClick(num)}
                variant="outline"
                className="border-purple-400 dark:border-purple-600 text-purple-600 dark:text-purple-400 hover:bg-purple-200 dark:hover:bg-purple-700"
                style={{ aspectRatio: '1', width: '100%' }}
              >
                {num}
              </Button>
            ))}
          </div>
        </div>
      </div>
    </>
  )
}
