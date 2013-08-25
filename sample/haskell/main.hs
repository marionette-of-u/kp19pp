import Parser

main =
  let
    p1 = post initParsingData Id 12
    p2 = post p1 Pls 0
    p3 = post p2 Id  9
    p4 = post p3 Ast 0
    p5 = post p4 Id  2
    p6 = post p5 End 0
    v = acceptedValue p6
  in putStrLn (show v)

