-- Lexing ... OK
+
+
@@ Tokens @@
List[281]:
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
  identifier(to_spill)
  =
  identifier(id)
  (
  const int(1)
  )
  ;
  int
  identifier(a)
  =
  identifier(id)
  (
  const int(2)
  )
  ;
  int
  identifier(b)
  =
  identifier(id)
  (
  const int(3)
  )
  ;
  int
  identifier(c)
  =
  identifier(id)
  (
  const int(4)
  )
  ;
  int
  identifier(d)
  =
  identifier(id)
  (
  const int(5)
  )
  ;
  int
  identifier(e)
  =
  identifier(id)
  (
  const int(6)
  )
  ;
  identifier(check_one_int)
  (
  identifier(to_spill)
  ,
  const int(1)
  )
  ;
  identifier(check_5_ints)
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
  const int(2)
  )
  ;
  identifier(check_5_ints)
  (
  const int(1)
  +
  identifier(a)
  ,
  const int(1)
  +
  identifier(b)
  ,
  const int(1)
  +
  identifier(c)
  ,
  const int(1)
  +
  identifier(d)
  ,
  const int(1)
  +
  identifier(e)
  ,
  const int(3)
  )
  ;
  identifier(check_one_int)
  (
  identifier(to_spill)
  ,
  const int(1)
  )
  ;
  int
  identifier(f)
  =
  identifier(id)
  (
  const int(7)
  )
  ;
  int
  identifier(g)
  =
  identifier(id)
  (
  const int(8)
  )
  ;
  int
  identifier(h)
  =
  identifier(id)
  (
  const int(9)
  )
  ;
  int
  identifier(i)
  =
  identifier(id)
  (
  const int(10)
  )
  ;
  int
  identifier(j)
  =
  identifier(id)
  (
  const int(11)
  )
  ;
  identifier(check_5_ints)
  (
  identifier(f)
  ,
  identifier(g)
  ,
  identifier(h)
  ,
  identifier(i)
  ,
  identifier(j)
  ,
  const int(7)
  )
  ;
  identifier(check_5_ints)
  (
  const int(1)
  +
  identifier(f)
  ,
  const int(1)
  +
  identifier(g)
  ,
  const int(1)
  +
  identifier(h)
  ,
  const int(1)
  +
  identifier(i)
  ,
  const int(1)
  +
  identifier(j)
  ,
  const int(8)
  )
  ;
  identifier(check_one_int)
  (
  identifier(to_spill)
  ,
  const int(1)
  )
  ;
  return
  const int(0)
  ;
  }
