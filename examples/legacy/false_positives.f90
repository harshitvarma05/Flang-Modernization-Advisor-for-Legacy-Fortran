program false_positives
  implicit none
  integer :: i
  real :: a(10), b
  i = 1
  if (a(i) > 0.0) then
    b = a(i)
  else
    b = 0.0
  end if
  goto 100
100 continue
  a(i) = b
end program false_positives
