-- Lexing ... OK
+
+
@@ Tokens @@
List[94]:
  int
  identifier(f)
  (
  int
  identifier(reg1)
  ,
  int
  identifier(reg2)
  ,
  int
  identifier(reg3)
  ,
  int
  identifier(reg4)
  ,
  int
  identifier(reg5)
  ,
  int
  identifier(reg6)
  ,
  int
  identifier(stack1)
  ,
  int
  identifier(stack2)
  ,
  int
  identifier(stack3)
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  if
  (
  identifier(reg1)
  ==
  const int(1)
  &&
  identifier(reg2)
  ==
  const int(2)
  &&
  identifier(reg3)
  ==
  const int(3)
  &&
  identifier(reg4)
  ==
  const int(4)
  &&
  identifier(reg5)
  ==
  const int(5)
  &&
  identifier(reg6)
  ==
  const int(6)
  &&
  identifier(stack1)
  ==
  -
  const int(1)
  &&
  identifier(stack2)
  ==
  -
  const int(2)
  &&
  identifier(stack3)
  ==
  -
  const int(3)
  &&
  identifier(x)
  ==
  const int(10)
  )
  {
  identifier(stack2)
  =
  const int(100)
  ;
  return
  identifier(stack2)
  ;
  }
  return
  const int(0)
  ;
  }
