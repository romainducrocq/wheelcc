-- Lexing ... OK
+
+
@@ Tokens @@
List[107]:
  int
  identifier(i)
  =
  const int(0)
  ;
  int
  identifier(j)
  =
  const int(0)
  ;
  int
  identifier(incr_i)
  (
  void
  )
  {
  if
  (
  identifier(i)
  ==
  const int(1)
  )
  {
  identifier(i)
  ++
  ;
  ++
  identifier(i)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(decr_j)
  (
  void
  )
  {
  if
  (
  identifier(j)
  ==
  -
  const int(1)
  )
  {
  identifier(j)
  --
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(i)
  ++
  ?
  const int(0)
  :
  identifier(incr_i)
  (
  )
  ;
  if
  (
  identifier(i)
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  --
  identifier(j)
  ?
  identifier(decr_j)
  (
  )
  :
  const int(0)
  ;
  if
  (
  identifier(j)
  !=
  -
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
