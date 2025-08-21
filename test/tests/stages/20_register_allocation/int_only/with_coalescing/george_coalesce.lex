-- Lexing ... OK
+
+
@@ Tokens @@
List[480]:
  int
  identifier(check_one_int)
  (
  int
  identifier(actual)
  ,
  int
  identifier(expected)
  )
  ;
  int
  identifier(check_5_ints)
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
  identifier(start)
  )
  ;
  int
  identifier(check_12_ints)
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
  ,
  int
  identifier(g)
  ,
  int
  identifier(h)
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(k)
  ,
  int
  identifier(l)
  ,
  int
  identifier(start)
  )
  ;
  int
  identifier(id)
  (
  int
  identifier(x)
  )
  ;
  int
  identifier(glob)
  =
  const int(1)
  ;
  int
  identifier(increment_glob)
  (
  void
  )
  {
  identifier(glob)
  =
  identifier(glob)
  +
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
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
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(c)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  const int(4)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(e)
  !=
  const int(5)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(f)
  !=
  const int(6)
  )
  {
  return
  const int(6)
  ;
  }
  int
  identifier(one)
  =
  identifier(glob)
  *
  const int(1)
  ;
  int
  identifier(two)
  =
  identifier(glob)
  *
  const int(2)
  ;
  int
  identifier(three)
  =
  identifier(glob)
  *
  const int(3)
  ;
  int
  identifier(four)
  =
  identifier(glob)
  *
  const int(4)
  ;
  if
  (
  identifier(one)
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(two)
  !=
  const int(2)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(three)
  !=
  const int(3)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(four)
  !=
  const int(4)
  )
  {
  return
  const int(10)
  ;
  }
  identifier(increment_glob)
  (
  )
  ;
  int
  identifier(five)
  =
  const int(4)
  +
  identifier(one)
  ;
  int
  identifier(six)
  =
  const int(4)
  +
  identifier(two)
  ;
  int
  identifier(seven)
  =
  const int(4)
  +
  identifier(three)
  ;
  int
  identifier(eight)
  =
  const int(4)
  +
  identifier(four)
  ;
  if
  (
  identifier(five)
  !=
  const int(5)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(six)
  !=
  const int(6)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(seven)
  !=
  const int(7)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(eight)
  !=
  const int(8)
  )
  {
  return
  const int(14)
  ;
  }
  identifier(increment_glob)
  (
  )
  ;
  int
  identifier(nine)
  =
  const int(14)
  -
  identifier(five)
  ;
  int
  identifier(ten)
  =
  const int(16)
  -
  identifier(six)
  ;
  int
  identifier(eleven)
  =
  const int(18)
  -
  identifier(seven)
  ;
  int
  identifier(twelve)
  =
  const int(20)
  -
  identifier(eight)
  ;
  identifier(increment_glob)
  (
  )
  ;
  if
  (
  identifier(nine)
  !=
  const int(9)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(ten)
  !=
  const int(10)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  identifier(eleven)
  !=
  const int(11)
  )
  {
  return
  const int(17)
  ;
  }
  if
  (
  identifier(twelve)
  !=
  const int(12)
  )
  {
  return
  const int(18)
  ;
  }
  int
  identifier(s)
  =
  identifier(glob)
  -
  const int(3)
  ;
  int
  identifier(t)
  =
  identifier(glob)
  -
  const int(2)
  ;
  int
  identifier(u)
  =
  identifier(glob)
  -
  const int(1)
  ;
  int
  identifier(v)
  =
  identifier(glob)
  *
  const int(2)
  -
  const int(4)
  ;
  int
  identifier(w)
  =
  identifier(glob)
  +
  const int(1)
  ;
  identifier(check_5_ints)
  (
  identifier(s)
  ,
  identifier(t)
  ,
  identifier(u)
  ,
  identifier(v)
  ,
  identifier(w)
  ,
  const int(1)
  )
  ;
  return
  identifier(check_one_int)
  (
  identifier(glob)
  ,
  const int(4)
  )
  ;
  }
