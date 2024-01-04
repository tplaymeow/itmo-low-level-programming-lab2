update Users
set rating = 5.0
where (age > 18 and age <= 50) or is_admin == true;
