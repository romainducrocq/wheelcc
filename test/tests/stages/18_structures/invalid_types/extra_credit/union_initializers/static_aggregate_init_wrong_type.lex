-- Lexing ... OK
+
+
@@ Tokens @@
List[60]:
  struct
  identifier(one_elem)
  {
  long
  identifier(l)
  ;
  }
  ;
  struct
  identifier(three_elems)
  {
  int
  identifier(one)
  ;
  int
  identifier(two)
  ;
  int
  identifier(three)
  ;
  }
  ;
  union
  identifier(one_or_three_elems)
  {
  struct
  identifier(one_elem)
  identifier(a)
  ;
  struct
  identifier(three_elems)
  identifier(b)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  static
  union
  identifier(one_or_three_elems)
  identifier(my_union)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  }
  ;
  return
  const int(0)
  ;
  }
