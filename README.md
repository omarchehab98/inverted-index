# Inverted Index

## Usage

```
$ echo "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras id felis lectus. Quisque dictum blandit consequat. Nulla facilisis ante augue, vitae lobortis elit lobortis finibus." > lipsum.txt

$ ./inverted_index lipsum.txt
{
    "adipiscing": [40],
    "nulla": [113],
    "blandit": [94],
    "lorem": [0],
    "dolor": [12],
    "dictum": [87],
    "amet": [22],
    "ante": [129],
    "cras": [57],
    "id": [62],
    "quisque": [79],
    "ipsum": [6],
    "elit": [51, 156],
    "lobortis": [147, 161],
    "consectetur": [28],
    "sit": [18],
    "lectus": [71],
    "finibus": [170],
    "felis": [65],
    "consequat": [102],
    "facilisis": [119],
    "augue": [134],
    "vitae": [141]
}
```
