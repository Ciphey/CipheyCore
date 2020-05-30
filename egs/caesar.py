import CipheyCore

test_string = "te hld esp ozgpc czlo esle wlj, zy l qctolj ytrse wlep ty yzgpxmpc, mpqzcp esp qtcde zq esp apcdzyd htes hszx estd stdezcj sld mfdtypdd."
analysis = CipheyCore.analyse_string(test_string)
expected = {
  "e": 11.162e-2,
  "t": 9.356e-2,
  "a": 8.497e-2,
  "r": 7.587e-2,
  "i": 7.546e-2,
  "o": 7.507e-2,
  "n": 6.749e-2,
  "s": 6.327e-2,
  "h": 6.094e-2,
  "d": 4.253e-2,
  "l": 4.025e-2,
  "u": 2.758e-2,
  "w": 2.560e-2,
  "m": 2.406e-2,
  "f": 2.228e-2,
  "c": 2.202e-2,
  "g": 2.015e-2,
  "y": 1.994e-2,
  "p": 1.929e-2,
  "b": 1.492e-2,
  "k": 1.292e-2,
  "v": 0.978e-2,
  "j": 0.153e-2,
  "x": 0.150e-2,
  "q": 0.095e-2,
  "z": 0.077e-2
}
group = list("abcdefghijklmnopqrstuvwxyz")
res = CipheyCore.caesar_crack(analysis, expected, group, True)
print(res[0].key == 11)
