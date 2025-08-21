-- Lexing ... OK
+
+
@@ Tokens @@
List[87]:
  long
  identifier(sign_extend)
  (
  int
  identifier(i)
  ,
  long
  identifier(expected)
  )
  {
  long
  identifier(extended)
  =
  (
  long
  )
  identifier(i)
  ;
  return
  (
  identifier(extended)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(sign_extend)
  (
  const int(10)
  ,
  const long(10l)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(sign_extend)
  (
  -
  const int(10)
  ,
  -
  const long(10l)
  )
  )
  {
  return
  const int(2)
  ;
  }
  long
  identifier(l)
  =
  (
  long
  )
  const int(100)
  ;
  if
  (
  identifier(l)
  !=
  const long(100l)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
