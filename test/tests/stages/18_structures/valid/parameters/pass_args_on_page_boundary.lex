-- Lexing ... OK
+
+
@@ Tokens @@
List[250]:
  struct
  identifier(nine_bytes)
  {
  char
  identifier(arr)
  [
  const int(11)
  ]
  ;
  }
  ;
  extern
  struct
  identifier(nine_bytes)
  identifier(on_page_boundary)
  ;
  int
  identifier(f)
  (
  struct
  identifier(nine_bytes)
  identifier(in_reg)
  ,
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  struct
  identifier(nine_bytes)
  identifier(on_stack)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(9)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  char
  identifier(in_reg_c)
  =
  identifier(in_reg)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  ;
  char
  identifier(on_stack_c)
  =
  identifier(on_stack)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  ;
  if
  (
  identifier(i)
  ==
  const int(2)
  )
  {
  if
  (
  identifier(in_reg_c)
  !=
  const int(4)
  ||
  identifier(on_stack_c)
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(3)
  )
  {
  if
  (
  identifier(in_reg_c)
  !=
  const int(5)
  ||
  identifier(on_stack_c)
  !=
  const int(5)
  )
  {
  return
  const int(2)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(8)
  )
  {
  if
  (
  identifier(in_reg_c)
  !=
  const int(6)
  ||
  identifier(on_stack_c)
  !=
  const int(6)
  )
  {
  return
  const int(3)
  ;
  }
  }
  else
  {
  if
  (
  identifier(in_reg_c)
  ||
  identifier(on_stack_c)
  )
  {
  return
  const int(4)
  ;
  }
  }
  }
  if
  (
  identifier(a)
  !=
  const int(101)
  ||
  identifier(b)
  !=
  const int(102)
  ||
  identifier(c)
  !=
  const int(103)
  ||
  identifier(d)
  !=
  const int(104)
  ||
  identifier(e)
  !=
  const int(105)
  )
  {
  return
  const int(5)
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
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(2)
  ]
  =
  const int(4)
  ;
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(3)
  ]
  =
  const int(5)
  ;
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(8)
  ]
  =
  const int(6)
  ;
  return
  identifier(f)
  (
  identifier(on_page_boundary)
  ,
  const int(101)
  ,
  const int(102)
  ,
  const int(103)
  ,
  const int(104)
  ,
  const int(105)
  ,
  identifier(on_page_boundary)
  )
  ;
  }
