# FixMemPool
fixed-block size memory pool, doubly linking with index in an array of object.
array (contigious in memory) is declared before pool initiliazation,
hence array is store out side the Pool.
no BLOCK object needed, but drawback shown when returning pointers to array of block.
no reset Pool, outPool allocation for larger item omitted,
no derivative considered,
all codes  are within this single file, brief test at the end.
//TODO: to debug on listNode scope and index-pointer conversion related issues.
