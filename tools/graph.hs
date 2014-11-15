#!/usr/bin/env runhaskell

import Data.List            (isPrefixOf)
import System.Environment   (getArgs)
import System.FilePath.Find
import Text.Printf          (printf)

inc :: Bool -> String -> [String]
inc f = (if f then id else filter (isPrefixOf "vb/")) . map (drop 10 . init) . filter (isPrefixOf "#include") . lines

main :: IO ()
main = do
    args <- getArgs
    putStrLn "digraph {"
    putStrLn "  rankdir = LR"
    files <- find always (extension ==? ".h") "libvb"
    mapM_ (print . drop 6) files
    contents <- mapM readFile files
    let arrows = concat $ zipWith (map . (,)) (map (drop 6) files) $ map (inc ("-f" `elem` args)) contents
    mapM_ (uncurry (printf "  \"%s\" -> \"%s\"\n")) arrows
    putStrLn "}"
