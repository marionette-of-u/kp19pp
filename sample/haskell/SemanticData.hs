module SemanticData (MyType, defaultValue, makeUMns, makeMlt, makeDiv, makeAdd, makeSub, makeId) where

type MyType = Integer
defaultValue = 0

makeAdd  x y = x + y
makeSub  x y = x - y
makeMlt  x y = x * y
makeDiv  x y = div x y
makeId   x   = x
makeUMns x   = -x

