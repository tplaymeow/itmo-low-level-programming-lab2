select from Abobus
join User on name == abobus_name
where name == "Avtobus"
  and (color CONTAINS "Red" or "Blue" contains color)
  and age >= 3;