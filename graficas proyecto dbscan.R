#proyecto apertura de Paralelo
library(ggplot2)

datos <- read.csv("ITAM/Comp Paralelo y Nube/proyecto 1 dbscan/resultados_finales.csv", header = FALSE)
datos <- datos[-1, c("V1", "V2", "V3", "V4")]
colnames(datos) <- c("num_datos", "num_threads", "tiempo_promedio", "speedups")
#datos$num_datos <- as.numeric(datos$num_datos)
datos$num_threads <- as.numeric(datos$num_threads)
datos$tiempo_promedio <- as.numeric(datos$tiempo_promedio)
datos$speedups <- as.numeric(datos$speedups)

#tiempos
ggplot(data= datos, aes(x=num_threads, y=tiempo_promedio, color= num_datos)) +
  geom_point() +
  geom_line() +
  scale_color_discrete(limits = unique(datos$num_datos))

#speedups
ggplot(data= datos, aes(x=num_threads, y=speedups, color= num_datos)) +
  geom_pint() +
  geom_line() +
  scale_color_discrete(limits = unique(datos$num_datos))

#speedups barras
ggplot(data = datos, aes(x = factor(num_threads), y = speedups, fill = num_datos)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(x = "Número de Threads", y = "Speedups") +
  scale_fill_brewer(palette = "Set2") +
  scale_color_discrete(limits = unique(datos$num_datos))# Opciones para elegir un esquema de colores

