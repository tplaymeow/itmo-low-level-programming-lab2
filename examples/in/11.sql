select from Abobus
where name == "Avtobus"
  and (color CONTAINS "Red" or "Blue" contains color)
  and age >= 35;