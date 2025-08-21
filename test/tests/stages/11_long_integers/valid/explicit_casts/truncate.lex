-- Lexing ... OK
+
+
@@ Tokens @@
List[116]:
  int
  identifier(truncate)
  (
  long
  identifier(l)
  ,
  int
  identifier(expected)
  )
  {
  int
  identifier(result)
  =
  (
  int
  )
  identifier(l)
  ;
  return
  (
  identifier(result)
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
  identifier(truncate)
  (
  const long(10l)
  ,
  const int(10)
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
  identifier(truncate)
  (
  -
  const long(10l)
  ,
  -
  const int(10)
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(truncate)
  (
  const long(17179869189l)
  ,
  const int(5)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(truncate)
  (
  -
  const long(17179869179l)
  ,
  const long(5l)
  )
  )
  {
  return
  const int(4)
  ;
  }
  int
  identifier(i)
  =
  (
  int
  )
  const long(17179869189l)
  ;
  if
  (
  identifier(i)
  !=
  const int(5)
  )
  return
  const int(5)
  ;
  return
  const int(0)
  ;
  }
