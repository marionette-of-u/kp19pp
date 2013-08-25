module Parser (Token(..), ParsingData(..), initParsingData, post, resultAccept, resultError) where
import SemanticData

type Value = MyType
data Token = End | UMns | Ast | Sla | Pls | Mns | LParen | RParen | Id
  deriving (Enum, Eq)

data StackFrame = StackFrame {
  state :: ParsingData -> Token -> Value -> (ParsingData, Bool),
  gotof :: ParsingData -> Int   -> Value -> (ParsingData, Bool),
  value :: Value
}

data Stack = Stack {
  stack :: [StackFrame],
  tmp   :: [StackFrame],
  gap   :: Int
}

data ParsingData = ParsingData {
  accepted      :: Bool,
  parsingError  :: Bool,
  acceptedValue :: Value,
  parsingStack  :: Stack
}

resetTmp  s   = Stack (stack s) [] (length (stack s))
commitTmp s   = Stack ((tmp s) ++ (drop ((length (stack s)) - (gap s)) (stack s))) (tmp s) (gap s)
push      s f = Stack (stack s) (f : (tmp s)) (gap s)

pop s n =
  if (length (tmp s)) < n
    then Stack (stack s) [] ((gap s) - (n - (length (tmp s))))
    else Stack (stack s) (drop n (tmp s)) (gap s)

top s =
  if (length (tmp s)) > 0
    then head (tmp s)
    else (stack s) !! ((length (stack s)) - 1 - ((gap s) - 1))

getArg s b i =
  let n = length (tmp s)
  in if (b - i) <= n
    then (tmp s)   !! (n - 1 - (n - (b - i)))
    else (stack s) !! ((length (stack s)) - 1 - (gap s) + (b - n) - i)

clear       s     = Stack [] (tmp s) (gap s)
stackTop    p     = top (parsingStack p)
getStackArg p b i = value (getArg (parsingStack p) b i)

pushStack      p s g v = ParsingData (accepted p) (parsingError p) (acceptedValue p) (push (parsingStack p) (StackFrame s g v))
popStack       p n     = ParsingData (accepted p) (parsingError p) (acceptedValue p) (pop (parsingStack p) n)
clearStack     p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (clear (parsingStack p))
resetTmpStack  p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (resetTmp (parsingStack p))
commitTmpStack p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (commitTmp (parsingStack p))

reset           = commitTmpStack (pushStack (ParsingData False False defaultValue (Stack [] [] 0)) state0 gotof0 defaultValue)
initParsingData = reset

iteration p t v =
  let n = (state (stackTop p)) p t v
      tFirst  (a, _) = a
      tSecond (_, b) = b
  in if tSecond n
    then iteration (tFirst n) t v
    else (tFirst n)

wrappedCommit p =
  let p' = commitTmpStack p
  in p'

post p t v =
  let p' = iteration (commitTmpStack p) t v
  in if parsingError p'
    then p'
    else wrappedCommit p'

resultAccept p = accepted p
resultError  p = parsingError p

call0makeUMns p i b i0 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeUMns (getStackArg p b i0))

call0makeMlt p i b i0 i1 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeMlt (getStackArg p b i0) (getStackArg p b i1))

call0makeDiv p i b i0 i1 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeDiv (getStackArg p b i0) (getStackArg p b i1))

call0makeAdd p i b i0 i1 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeAdd (getStackArg p b i0) (getStackArg p b i1))

call0makeSub p i b i0 i1 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeSub (getStackArg p b i0) (getStackArg p b i1))

call0makeId p i b i0 =
  let p' = popStack p b
  in (gotof (stackTop p')) p' i (makeId (getStackArg p b i0))

gotof0 p i v =
  case i of
    0 -> (pushStack p state13 gotof13 v, True)
    _ -> (p, False)

state0 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof1 p i v =
  case i of
    0 -> (pushStack p state7 gotof7 v, True)
    _ -> (p, False)

state1 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof2 p i v =
  case i of
    0 -> (pushStack p state8 gotof8 v, True)
    _ -> (p, False)

state2 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof3 p i v =
  case i of
    0 -> (pushStack p state9 gotof9 v, True)
    _ -> (p, False)

state3 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof4 p i v =
  case i of
    0 -> (pushStack p state10 gotof10 v, True)
    _ -> (p, False)

state4 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof5 p i v =
  case i of
    0 -> (pushStack p state11 gotof11 v, True)
    _ -> (p, False)

state5 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof6 p i v =
  case i of
    0 -> (pushStack p state12 gotof12 v, True)
    _ -> (p, False)

state6 p t v =
  case t of
    Mns    -> (pushStack p state1  gotof1  v, False)
    LParen -> (pushStack p state2  gotof2  v, False)
    Id     -> (pushStack p state14 gotof14 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof7 p i v =
  case i of
    _ -> (p, True)

state7 p t v =
  case t of
    Ast    -> call0makeUMns p 0 2 1
    Sla    -> call0makeUMns p 0 2 1
    Pls    -> call0makeUMns p 0 2 1
    Mns    -> call0makeUMns p 0 2 1
    RParen -> call0makeUMns p 0 2 1
    End    -> call0makeUMns p 0 2 1
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof8 p i v =
  case i of
    _ -> (p, True)

state8 p t v =
  case t of
    Ast    -> (pushStack p state3  gotof3  v, False)
    Sla    -> (pushStack p state4  gotof4  v, False)
    Pls    -> (pushStack p state5  gotof5  v, False)
    Mns    -> (pushStack p state6  gotof6  v, False)
    RParen -> (pushStack p state15 gotof15 v, False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof9 p i v =
  case i of
    _ -> (p, True)

state9 p t v =
  case t of
    Ast    -> call0makeMlt p 0 3 0 2
    Sla    -> call0makeMlt p 0 3 0 2
    Pls    -> call0makeMlt p 0 3 0 2
    Mns    -> call0makeMlt p 0 3 0 2
    RParen -> call0makeMlt p 0 3 0 2
    End    -> call0makeMlt p 0 3 0 2
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof10 p i v =
  case i of
    _ -> (p, True)

state10 p t v =
  case t of
    Ast    -> call0makeDiv p 0 3 0 2
    Sla    -> call0makeDiv p 0 3 0 2
    Pls    -> call0makeDiv p 0 3 0 2
    Mns    -> call0makeDiv p 0 3 0 2
    RParen -> call0makeDiv p 0 3 0 2
    End    -> call0makeDiv p 0 3 0 2
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof11 p i v =
  case i of
    _ -> (p, True)

state11 p t v =
  case t of
    Ast    -> (pushStack p state3 gotof3 v, False)
    Sla    -> (pushStack p state4 gotof4 v, False)
    Pls    -> call0makeAdd p 0 3 0 2
    Mns    -> call0makeAdd p 0 3 0 2
    RParen -> call0makeAdd p 0 3 0 2
    End    -> call0makeAdd p 0 3 0 2
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof12 p i v =
  case i of
    _ -> (p, True)

state12 p t v =
  case t of
    Ast    -> (pushStack p state3 gotof3 v, False)
    Sla    -> (pushStack p state4 gotof4 v, False)
    Pls    -> call0makeSub p 0 3 0 2
    Mns    -> call0makeSub p 0 3 0 2
    RParen -> call0makeSub p 0 3 0 2
    End    -> call0makeSub p 0 3 0 2
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof13 p i v =
  case i of
    _ -> (p, True)

state13 p t v =
  case t of
    Ast    -> (pushStack p state3 gotof3 v, False)
    Sla    -> (pushStack p state4 gotof4 v, False)
    Pls    -> (pushStack p state5 gotof5 v, False)
    Mns    -> (pushStack p state6 gotof6 v, False)
    End    -> (ParsingData True (parsingError p) (getStackArg p 1 0) (parsingStack p), False)
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof14 p i v =
  case i of
    _ -> (p, True)

state14 p t v =
  case t of
    Ast    -> call0makeId p 0 1 0
    Sla    -> call0makeId p 0 1 0
    Pls    -> call0makeId p 0 1 0
    Mns    -> call0makeId p 0 1 0
    RParen -> call0makeId p 0 1 0
    End    -> call0makeId p 0 1 0
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

gotof15 p i v =
  case i of
    _ -> (p, True)

state15 p t v =
  case t of
    Ast    -> call0makeId p 0 3 1
    Sla    -> call0makeId p 0 3 1
    Pls    -> call0makeId p 0 3 1
    Mns    -> call0makeId p 0 3 1
    RParen -> call0makeId p 0 3 1
    End    -> call0makeId p 0 3 1
    _      -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)

-- module SemanticData (MyType, defaultValue, makeUMns, makeMlt, makeDiv, makeAdd, makeSub, makeId) where
