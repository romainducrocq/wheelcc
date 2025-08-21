-- Lexing ... OK
+
+
@@ Tokens @@
List[55]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  int
  identifier(c)
  ;
  }
  ;
  struct
  identifier(s)
  identifier(global_struct)
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  int
  *
  identifier(target)
  (
  void
  )
  {
  return
  &
  identifier(global_struct)
  .
  identifier(b)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  *
  identifier(target)
  (
  )
  ==
  const int(2)
  ;
  }
