-- Lexing ... OK
+
+
@@ Tokens @@
List[117]:
  struct
  identifier(inner)
  {
  int
  identifier(inner_arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(outer)
  {
  int
  identifier(a)
  ;
  struct
  identifier(inner)
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
  struct
  identifier(outer)
  identifier(array)
  [
  const int(4)
  ]
  =
  {
  {
  const int(1)
  ,
  {
  {
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  }
  }
  ,
  {
  const int(5)
  ,
  {
  {
  const int(6)
  ,
  const int(7)
  ,
  const int(8)
  }
  }
  }
  ,
  {
  const int(9)
  ,
  {
  {
  const int(10)
  ,
  const int(11)
  ,
  const int(12)
  }
  }
  }
  ,
  {
  const int(13)
  ,
  {
  {
  const int(14)
  ,
  const int(15)
  ,
  const int(16)
  }
  }
  }
  }
  ;
  int
  identifier(i)
  =
  -
  identifier(array)
  [
  const int(2)
  ]
  .
  identifier(b)
  .
  identifier(inner_arr)
  [
  const int(1)
  ]
  ;
  return
  identifier(i)
  ==
  -
  const int(11)
  ;
  }
