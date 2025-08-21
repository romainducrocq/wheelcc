-- Lexing ... OK
+
+
@@ Tokens @@
List[165]:
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(w)
  =
  const int(3)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(w)
  ++
  ;
  }
  int
  identifier(x)
  =
  const int(10)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(x)
  --
  ;
  }
  int
  identifier(y)
  =
  -
  const int(12)
  ;
  if
  (
  identifier(flag)
  )
  {
  ++
  identifier(y)
  ;
  }
  int
  identifier(z)
  =
  -
  const int(100)
  ;
  if
  (
  identifier(flag)
  )
  {
  --
  identifier(z)
  ;
  }
  if
  (
  identifier(flag)
  )
  {
  if
  (
  identifier(w)
  ==
  const int(4)
  &&
  identifier(x)
  ==
  const int(9)
  &&
  identifier(y)
  ==
  -
  const int(11)
  &&
  identifier(z)
  ==
  -
  const int(101)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  else
  {
  if
  (
  identifier(w)
  ==
  const int(3)
  &&
  identifier(x)
  ==
  const int(10)
  &&
  identifier(y)
  ==
  -
  const int(12)
  &&
  identifier(z)
  ==
  -
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(target)
  (
  const int(0)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target)
  (
  const int(1)
  )
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
