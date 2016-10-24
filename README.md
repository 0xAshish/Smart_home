# Smart_home                                                                                                                    
this project is kind of basic automation in my room.                                                                              
functions                                                                                                                         
1.IR controls with tv remote                                                                              
2.Automatic lights on with motion detection                                                                              
3.Data uploading to webserver                                                                              
4.16*2 lcd display                                                                              
5.water tank level measurment using ultrasonic sensor                                                                             
....                                                                               
1                                                                              
  get some values from IR transmitter from remote and triger relay accordingly
  in this project  hvae used IR libraray                                                                              
2                                                                              
  one PIR motion detection sensor is located on door frame and one LDR is also connected to Arduino                              
  so when there is no lights in room and motion is detected then light1 will on(there is safe button for night)        
3                                                                                
  wifi esp8266 is used to update data on webserver using php form on webserver                                   
  and basic http packet formet of http for form data is used.                                  
  for more detail see sendhttp() function                                                                               
4                                                                               
  lcd is basic output formet for debugging and monitor purpose                                  
  it shows date , time, water level, motion count and sound detection count                                  
5                                                                               
   ultrasonic sensor measures distance between water surface and itself and by using whole length
   and logic-> program shows it in 0-100% formet                                  
   
   
   
  TODO
  android app for monitoring
  and control only if it is connected to same router(mobile and system)
   
