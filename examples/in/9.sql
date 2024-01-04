select from Users
where (age > 18 and age <= 50) or (is_admin == true or name == "Admin" and age != 10);
