#import "@preview/lilaq:0.5.0" as lq

#set page(paper: "a5", margin: (top: 12mm, bottom: 14mm, left: 12mm, right: 12mm))
#set text(font: "Libertinus Serif", size: 10.0pt)
#set par(leading: 0.5em, justify: true)

= Laufzeit-Messungen $t(n)$

#v(2mm)

Gemessene Werte der Laufzeit bei Berechnung der Eigenwerten von mehreren $bb(R)^(n times n)$ Matrizen.
Die Matrizen wurden zufällig erstellt und erhielten genormte Einträge. Außerdem handelt es sich um nicht symmetrische Matrizen. Je Kleiner $n$ desto größere Wiederholungen wurden gewählt um die mittlere Laufzeit zu berechnen.

#let data = (
  (2,    0.00179396),
  (4,    0.007231516),
  (8,    0.02581844),
  (16,   0.09953436),
  (32,   0.5684658),
  (64,   2.7067147),
  (128,  20.8939),
  (256,  89.69149999999999),
  (512,  1555.6813333333332),
  (1024, 8787.564666666667),
  (2048, 57788.41766666667),
)

#let xs = data.map(p => p.at(0))
#let ys = data.map(p => p.at(1))

#let fmt(x) = str(calc.round(x, digits: 6))


#table(
  columns: (1fr, 1fr),
  inset: (x: 2.5mm, y: 1.5mm),
  stroke: (x: 0.2pt, y: 0.2pt, top: 0.2pt, bottom: 0.2pt),
  fill: (x, y) => if y == 0 { luma(0.93) } else { none },
  align: (center, center),

  [$n$], [$t(n)$ in ms],
  ..data.map(p => (str(p.at(0)), fmt(p.at(1)))).flatten()
)

Um beim Plot die $y$ Achse außeinander zu ziehen habe ich mich entschieden diese logarithmisch darzustellen.

#lq.diagram(
  width: 100%,
  height: 60mm,
  title: [Laufzeit $t(n)$],
  xlabel: [Matrixgröße $n$ (log, Basis 2)],
  ylabel: [Zeit pro Lauf [ms]],

  xlim: (2, 2048),
  xscale: lq.scale.log(base: 2),

  lq.plot(xs, ys, mark: "o"),
)

#let exps = (
  (2,    2.0111503997),
  (4,    1.8360318053),
  (8,    1.9467928078),
  (16,   2.5138070106),
  (32,   2.2513973674),
  (64,   2.9484670731),
  (128,  2.1018894613),
  (256,  4.1164314894),
  (512,  2.4979168328),
  (1024, 2.7172450613),
)

#let xp = exps.map(p => p.at(0))
#let yp = exps.map(p => p.at(1))

Um den Zusammenhang zwischen $n$ und der Laufzeit herauszufinden, kann man den Exponenten darstellen als $p(n)=log_2(f(n))$. So bekommen folgendes Ergebnis:

#v(2mm)
#align(center)[
  #table(
  columns: (auto, auto),
  inset: 10pt,

  align: center,
  table.header([$n$], [$p(n)$]),
  ..exps.map(p => (str(p.at(0)), fmt(p.at(1)))).flatten()
)
]

#v(2mm)

$=>$ Wir können Schlussfolgern dass unsere Exponenten Menge sich etwa kubisch verhällt.