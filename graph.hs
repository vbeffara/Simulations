#!/usr/bin/env runhaskell
{-# LANGUAGE OverloadedStrings, TupleSections #-}

import Data.List (isPrefixOf)
import qualified Data.Text as T
import qualified Data.Text.IO as TIO
import System.FilePath.Find
import Text.Printf (printf)

inc :: String -> [String]
inc = filter (isPrefixOf "vb/") . map (drop 10 . init) . filter (isPrefixOf "#include") . lines

main :: IO ()
main = do   
    putStrLn "digraph {"
    files <- find (fileName /=? "build") (extension ==? ".h") "."
    let names = map (drop 8) files
    mapM_ print names
    contents <- mapM readFile files
    let arrows = concat $ zipWith (map . (,)) names $ map inc contents
    mapM_ (uncurry (printf "  \"%s\" -> \"%s\"\n")) arrows
    putStrLn "}"
