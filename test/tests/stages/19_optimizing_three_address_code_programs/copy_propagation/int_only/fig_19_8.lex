-- Lexing ... OK
+
+
@@ Tokens @@
List[136]:
  static
  int
  identifier(called_counter)
  =
  const int(0)
  ;
  int
  identifier(callee)
  (
  int
  identifier(i)
  )
  {
  if
  (
  identifier(i)
  ==
  const int(3)
  &&
  identifier(called_counter)
  ==
  const int(0)
  )
  {
  identifier(called_counter)
  =
  const int(1)
  ;
  return
  const int(1)
  ;
  }
  if
  (
  identifier(i)
  ==
  const int(4)
  &&
  identifier(called_counter)
  ==
  const int(1)
  )
  {
  identifier(called_counter)
  =
  const int(2)
  ;
  return
  const int(0)
  ;
  }
  identifier(called_counter)
  =
  -
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(y)
  =
  const int(3)
  ;
  int
  identifier(keep_looping)
  ;
  do
  {
  identifier(keep_looping)
  =
  identifier(callee)
  (
  identifier(y)
  )
  ;
  identifier(y)
  =
  const int(4)
  ;
  }
  while
  (
  identifier(keep_looping)
  )
  ;
  return
  identifier(y)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(result)
  =
  identifier(target)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(called_counter)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
