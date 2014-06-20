#!/usr/bin/env runhaskell

import Data.List (isPrefixOf)
import System.FilePath.Find
import Text.Printf (printf)

inc :: String -> [String]
inc = filter (isPrefixOf "vb/") . map (drop 10 . init) . filter (isPrefixOf "#include") . lines

main :: IO ()
main = do   
    putStrLn "digraph {"
    files <- find always (extension ==? ".h") "libvb"
    mapM_ (print . drop 6) files
    contents <- mapM readFile files
    let arrows = concat $ zipWith (map . (,)) (map (drop 6) files) $ map inc contents
    mapM_ (uncurry (printf "  \"%s\" -> \"%s\"\n")) arrows
    putStrLn "}"
