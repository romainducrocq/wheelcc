-- Lexing ... OK
+
+
@@ Tokens @@
List[342]:
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
  identifier(target)
  (
  void
  )
  {
  int
  identifier(a)
  =
  identifier(id)
  (
  const int(1)
  )
  ;
  int
  identifier(b)
  =
  identifier(id)
  (
  const int(2)
  )
  ;
  int
  identifier(x)
  =
  identifier(id)
  (
  const int(10)
  )
  ;
  identifier(check_one_int)
  (
  identifier(x)
  ,
  const int(10)
  )
  ;
  int
  identifier(c)
  =
  identifier(x)
  ;
  if
  (
  !
  identifier(a)
  )
  {
  identifier(c)
  =
  const int(100)
  ;
  }
  int
  identifier(d)
  =
  identifier(id)
  (
  const int(3)
  )
  ;
  int
  identifier(e)
  =
  identifier(id)
  (
  const int(4)
  )
  ;
  int
  identifier(f)
  =
  identifier(id)
  (
  const int(5)
  )
  ;
  identifier(check_5_ints)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  ,
  const int(1)
  )
  ;
  identifier(check_5_ints)
  (
  identifier(a)
  +
  const int(3)
  ,
  identifier(b)
  +
  const int(3)
  ,
  identifier(d)
  +
  const int(3)
  ,
  identifier(e)
  +
  const int(3)
  ,
  identifier(f)
  +
  const int(3)
  ,
  const int(4)
  )
  ;
  identifier(check_5_ints)
  (
  identifier(a)
  +
  const int(4)
  ,
  identifier(b)
  +
  const int(4)
  ,
  identifier(d)
  +
  const int(4)
  ,
  identifier(e)
  +
  const int(4)
  ,
  identifier(f)
  +
  const int(4)
  ,
  const int(5)
  )
  ;
  identifier(check_one_int)
  (
  identifier(a)
  *
  const int(2)
  ,
  const int(2)
  )
  ;
  identifier(check_one_int)
  (
  identifier(b)
  *
  const int(2)
  ,
  const int(4)
  )
  ;
  identifier(check_one_int)
  (
  identifier(d)
  *
  const int(2)
  ,
  const int(6)
  )
  ;
  identifier(check_one_int)
  (
  identifier(e)
  *
  const int(2)
  ,
  const int(8)
  )
  ;
  identifier(check_one_int)
  (
  identifier(f)
  *
  const int(2)
  ,
  const int(10)
  )
  ;
  identifier(check_one_int)
  (
  identifier(a)
  *
  const int(3)
  ,
  const int(3)
  )
  ;
  identifier(check_one_int)
  (
  identifier(b)
  *
  const int(3)
  ,
  const int(6)
  )
  ;
  identifier(check_one_int)
  (
  identifier(d)
  *
  const int(3)
  ,
  const int(9)
  )
  ;
  identifier(check_one_int)
  (
  identifier(e)
  *
  const int(3)
  ,
  const int(12)
  )
  ;
  identifier(check_one_int)
  (
  identifier(f)
  *
  const int(3)
  ,
  const int(15)
  )
  ;
  int
  identifier(g)
  =
  identifier(c)
  ;
  if
  (
  !
  identifier(f)
  )
  {
  identifier(g)
  =
  -
  const int(1)
  ;
  }
  identifier(check_one_int)
  (
  identifier(g)
  ,
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
