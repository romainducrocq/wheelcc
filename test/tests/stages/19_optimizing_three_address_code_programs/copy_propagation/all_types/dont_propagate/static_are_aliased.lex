-- Lexing ... OK
+
+
@@ Tokens @@
List[80]:
  int
  identifier(stat)
  ;
  int
  identifier(target)
  (
  int
  *
  identifier(stat_ptr)
  )
  {
  int
  identifier(a)
  =
  const int(0)
  ;
  identifier(a)
  =
  identifier(stat)
  ;
  *
  identifier(stat_ptr)
  =
  const int(8)
  ;
  return
  identifier(a)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  *
  identifier(ptr)
  =
  &
  identifier(stat)
  ;
  identifier(stat)
  =
  const int(5)
  ;
  int
  identifier(result)
  =
  identifier(target)
  (
  identifier(ptr)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(5)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(stat)
  !=
  const int(8)
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
