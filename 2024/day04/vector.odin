package aoc

Vector :: distinct [2]int

Direction :: enum {
    North,     East,
    South,     West,
    Northwest, Northeast,
    Southwest, Southeast
}

/*
Brief
--------
 *  Use these constant vectors to perform matrix math at a higher level.
--------

Note
--------
*   Our word search is conceptually arranged such that [0, 0] is the top left.
```
    [0]    [1]    [2]    ...    [m]
[0] [0, 0] [1, 0] [2, 0]
[1] [0, 1] [1, 1] [2, 1]
[2] [0, 2] [1, 2] [2, 2]
...
[n] [0, n] [1, n] [2, n]        [m, n]
```
--------
 */
direction_vectors: [Direction]Vector = {
    .North     = { 0, -1},  .East      = {+1,  0},
    .South     = { 0, +1},  .West      = {-1,  0},
    .Northwest = {-1, -1},  .Northeast = {+1, -1},
    .Southwest = {-1, +1},  .Southeast = {+1, +1},
}

direction_diagonals := [?]Direction{.Northwest, .Northeast, .Southwest, .Southeast}

direction_complements: [Direction]Direction = {
    .North     = .South,     .East      = .West,
    .South     = .North,     .West      = .South,
    .Northwest = .Southeast, .Northeast = .Southwest,
    .Southwest = .Northeast, .Southeast = .Northwest
}

/*
    dimensions: [rows=3,cols=2]
        [0,0] [1,0] [2,0]
        [0,1] [1,1] [2,1]

    vector-to-index:
        [0,0] <=> 0 <=> 0 + 0*3
        [1,0] <=> 1 <=> 1 + 0*3
        [2,0] <=> 2 <=> 2 + 0*3
        [0,1] <=> 3 <=> 0 + 1*3
        [1,1] <=> 4 <=> 1 + 1*3
        [2,1] <=> 5 <=> 2 + 1*3
 */
vector_to_index :: proc(vec: Vector, frame: ^Frame) -> (index: int) {
    return vec.x + vec.y*frame.rows
}

/*
    dimensions: [rows=3,cols=2]
        [0,0] [1,0] [2,0]
        [0,1] [1,1] [2,1]

    index-to-vector:
    0 <=> [0,0] <=> [0%3 => 0, floor(0/3) => 0]
    1 <=> [1,0] <=> [1%3 => 1, floor(1/3) => 0]
    2 <=> [2,0] <=> [2%3 => 2, floor(2/3) => 0]
    3 <=> [0,1] <=> [3%3 => 0, floor(3/3) => 1]
    4 <=> [1,1] <=> [4%3 => 1, floor(4/3) => 1]
    5 <=> [2,1] <=> [5%3 => 2, floor(5/3) => 1]
 */
index_to_vector :: proc(idx: int, frame: ^Frame) -> (vector: Vector) {
    return {idx % frame.cols, idx / frame.rows}
}

/*
Case 1: Within Bounds to Within Bounds
--------
```
dimensions: [rows=3,cols=2]
    [0,0] [1,0] [2,0]*
    [0,1] [1,1] [2,1]
        pos := [2,0]
        dir := [0,+1] // direction_vectors[.South]
        res := [2,1]
```
--------

Case 2: Within Bounds to Outside Bounds
--------
```
dimensions: [rows=3,cols=2]
    [0,0] [1,0] [2,0]*
    [0,1] [1,1] [2,1]
        pos := [ 2, 0]
        dir := [+1, 0] // direction_vectors[.East]
        res := [ 3, 0] // Out of bounds!
```
--------
 */
vector_update :: proc(vec: ^Vector, dir: Direction, frame: ^Frame) -> bool {
    res := vec^ + direction_vectors[dir]
    ok  := vector_is_in_range(res, frame)
    if ok {
        vec^ = res
    }
    return ok
}

vector_is_in_range :: proc(vec: Vector, frame: ^Frame) -> bool {
    x_is_in_range := 0 <= vec.x && vec.x < frame.rows
    y_is_in_range := 0 <= vec.y && vec.y < frame.cols
    return x_is_in_range && y_is_in_range
}
