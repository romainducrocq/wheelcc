-- Lexing ... OK
+
+
@@ Tokens @@
List[649]:
  static
  int
  identifier(outer_flag)
  =
  const int(0)
  ;
  static
  int
  identifier(inner_flag)
  =
  const int(1)
  ;
  int
  identifier(inner_loop1)
  (
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
  int
  identifier(f)
  )
  {
  if
  (
  identifier(a)
  !=
  const int(1)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(12)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(20)
  ||
  identifier(f)
  !=
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
  int
  identifier(inner_loop2)
  (
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
  int
  identifier(f)
  )
  {
  if
  (
  identifier(outer_flag)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(a)
  !=
  const int(1)
  ||
  identifier(b)
  !=
  const int(2)
  ||
  identifier(c)
  !=
  const int(3)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(5)
  ||
  identifier(f)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  }
  else
  {
  if
  (
  identifier(a)
  !=
  const int(10)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(12)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(20)
  ||
  identifier(f)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  }
  return
  const int(1)
  ;
  }
  int
  identifier(inner_loop3)
  (
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
  int
  identifier(f)
  )
  {
  if
  (
  identifier(outer_flag)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(inner_flag)
  ==
  const int(2)
  )
  {
  if
  (
  identifier(a)
  !=
  const int(1)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(3)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(5)
  ||
  identifier(f)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  }
  else
  {
  if
  (
  identifier(a)
  !=
  const int(1)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(12)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(5)
  ||
  identifier(f)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  }
  }
  else
  {
  if
  (
  identifier(a)
  !=
  const int(10)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(12)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(20)
  ||
  identifier(f)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  }
  return
  const int(1)
  ;
  }
  int
  identifier(inner_loop4)
  (
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
  int
  identifier(f)
  )
  {
  return
  identifier(a)
  +
  identifier(b)
  +
  identifier(c)
  +
  identifier(d)
  +
  identifier(e)
  +
  identifier(f)
  ;
  }
  int
  identifier(validate)
  (
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
  int
  identifier(f)
  )
  {
  if
  (
  identifier(a)
  !=
  const int(10)
  ||
  identifier(b)
  !=
  const int(11)
  ||
  identifier(c)
  !=
  const int(12)
  ||
  identifier(d)
  !=
  const int(4)
  ||
  identifier(e)
  !=
  const int(20)
  ||
  identifier(f)
  !=
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
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(1)
  ;
  int
  identifier(b)
  =
  const int(2)
  ;
  int
  identifier(c)
  =
  const int(3)
  ;
  int
  identifier(d)
  =
  const int(4)
  ;
  int
  identifier(e)
  =
  const int(5)
  ;
  int
  identifier(f)
  =
  const int(100)
  ;
  while
  (
  identifier(outer_flag)
  <
  const int(2)
  )
  {
  while
  (
  identifier(inner_flag)
  <
  const int(1)
  )
  {
  if
  (
  !
  identifier(inner_loop1)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  )
  )
  {
  return
  const int(1)
  ;
  }
  identifier(a)
  =
  const int(10)
  ;
  identifier(inner_flag)
  =
  const int(1)
  ;
  }
  while
  (
  identifier(inner_flag)
  <
  const int(2)
  )
  {
  if
  (
  !
  identifier(inner_loop2)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  )
  )
  {
  return
  const int(2)
  ;
  }
  identifier(b)
  =
  const int(11)
  ;
  identifier(inner_flag)
  =
  const int(2)
  ;
  }
  while
  (
  identifier(inner_flag)
  <
  const int(4)
  )
  {
  if
  (
  !
  identifier(inner_loop3)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  )
  )
  {
  return
  const int(3)
  ;
  }
  identifier(inner_flag)
  =
  identifier(inner_flag)
  +
  const int(1)
  ;
  identifier(c)
  =
  const int(12)
  ;
  }
  while
  (
  identifier(inner_flag)
  <
  const int(4)
  )
  {
  identifier(inner_loop4)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  )
  ;
  identifier(d)
  =
  const int(13)
  ;
  }
  identifier(e)
  =
  const int(20)
  ;
  identifier(f)
  =
  const int(100)
  ;
  identifier(outer_flag)
  =
  identifier(outer_flag)
  +
  const int(1)
  ;
  identifier(inner_flag)
  =
  const int(0)
  ;
  }
  if
  (
  !
  identifier(validate)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  )
  )
  {
  return
  const int(4)
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
  return
  identifier(target)
  (
  )
  ;
  }
