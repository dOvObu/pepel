# pepel

Примеры

\ main()
  print("Oh, hi")
/



type Tree ( left: Tree; right: Tree; value = 0 )

\ printTree(t: Tree)
  print(t.value)
  printTree(t.left)
  printTree(t.right)
/



type List
(
  prev: List
  next: List
  value: Str = "some data"

  \ printList()
    print(value)
    next.printList()
  /

  \ printList2() print(value); next.printList2();;
  \ printList3() print(value); next.printList3(); /
  \ show() = print(value)
)
