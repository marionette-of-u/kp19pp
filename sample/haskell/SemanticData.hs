module SemanticData (MyType, defaultValue, makePls, makeSub, makeMlt, makeDiv, makeId, makeUMns) where

type MyType = Integer
defaultValue = 0

makePls  x y = x + y
makeSub  x y = x - y
makeMlt  x y = x * y
makeDiv  x y = div x y
makeId   x   = x
makeUMns x   = -x

