### CÓDIGO: Juego con pygame ###
### Modificado e implementado por Alejandro Gómez y Abraham Ortiz Castro ###
"""
Este ejercicio de SoC está basado en un banner web 'popular' modificado
para cubrir ciertas características solicitadas por la actividad.
"""

# Importamos los modulos a utilizar a través de la libreria pygame y os
import os, pygame
from pygame.locals import *

from pygame import mixer

# Mensajes de alerta de incompatibilidad generados en terminal
pygame.mixer.init()
if not pygame.font: print ('Warning, fonts disabled')
if not pygame.mixer: print ('Warning, sound disabled')

# Directivas os para devolver una tupla con dos componentes head y tail (línea 21)
# y unir varios componentes de ruta (línea 22)
main_dir = os.path.split(os.path.abspath(__file__))[0]
data_dir = os.path.join(main_dir, '/home/hectorsalamanca/juego')


# Funciones para crear los recursos necesarios para el juego a través de las
# librerias os y pygame
def load_image(name, colorkey=None):
    fullname = os.path.join(data_dir, name)
    try:
        image = pygame.image.load(fullname)
    except pygame.error:
        print ('Cannot load image:', fullname)
        raise SystemExit(str(pygame.get_error()))
    image = image.convert()
    if colorkey is not None:
        if colorkey == -1:
            colorkey = image.get_at((0,0))
        image.set_colorkey(colorkey, RLEACCEL)
    return image, image.get_rect()

# Clases para nuestros objetos en el juego
class Thwomp(pygame.sprite.Sprite):
    """mueve al thwomp sobre la pantalla, siguiendo al ratón"""
    def __init__(self):
        pygame.sprite.Sprite.__init__(self) # Llama al inicializador de sprite
        self.image = None
        self.rect = None        
        self.punching = 0
        self.load_image()
        
    def load_image(self):
        self.image, self.rect = load_image('tw.bmp', -1)
        
    def update(self):
        "Mueve al thwomp en función de la posición del ratón"
        pos = pygame.mouse.get_pos() # Obtiene la posición del ratón sobre la pantalla
        self.rect.midtop = pos
        if self.punching:
            self.rect.move_ip(5, 10)

    def punch(self, target):
        "Devuelve un valor true si el thwomp aplasta al objetivo"
        if not self.punching:
            self.punching = 1
            hitbox = self.rect.inflate(-5, -5)
            return hitbox.colliderect(target.rect)

    def unpunch(self):
        "Llamado al thwomp a retornar a su posición inicial sobre el mouse"
        self.punching = 0


class Mario(pygame.sprite.Sprite):
    """Mueve un sprite de Mario por la pantalla. El sprite girará cuando el
       thwomp lo aplaste."""
    def __init__(self):
        pygame.sprite.Sprite.__init__(self) # Llamado al intializador de sprite
        
        self.image = None
        self.rect = None 
        self.area = None
        self.move = 9
        self.movey = 18
        self.dizzy = 0
        self.load_image()
        screen = pygame.display.get_surface()
        self.area = screen.get_rect()
        self.rect.topleft = 10, 10
        
        
    def load_image(self):
        self.image, self.rect = load_image('Mario.bmp', -1)
        
        

    def update(self):
        "Actualiza el estado (caminar o girar), según el estado de Mario"
        if self.dizzy:
            self._spin() # Mario gira si es golpeado
        else:
            self._walk() # Mario cammina por la pantalla (estado base)

### MODIFICACIÓN I (Movimiento del sprite)
# Esta es una de las secciones con mayores modificaciones en contraste con el código
# base, pues esta vez el sprite deberá moverse por una pantalla de 800x800 en todas
# direcciones en lugar de solo ir recto de izquierda a derecha
    def _walk(self):
        "Mueve a Mario por la pantalla, retornando en los extremos de la misma"
        newpos = self.rect.move((self.move,self.movey))
        if self.rect.left < self.area.left or \
            self.rect.right > self.area.right:
            self.move = -self.move
            newpos = self.rect.move((self.move,self.movey))
            self.image = pygame.transform.flip(self.image, 1, 0)
        elif self.rect.top < self.area.top or \
            self.rect.bottom > self.area.bottom:
            self.movey = -self.movey
            newpos = self.rect.move((self.move,self.movey))
        self.rect = newpos

#### TERMINA MODIFICACIÓN ####

    def _spin(self):
        "Gira la imagen de Mario"
        center = self.rect.center
        self.dizzy = self.dizzy + 12
        if self.dizzy >= 360:
            self.dizzy = 0
            self.image = self.original
        else:
            rotate = pygame.transform.rotate
            self.image = rotate(self.original, self.dizzy)
        self.rect = self.image.get_rect(center=center)

    def punched(self):
        "Este es el parámetro que determinará que Mario gire o no"
        if not self.dizzy:
            self.dizzy = 1
            self.original = self.image


def main():
    """Esta función es la principal y se hace llamar cuando se inicia el programa.
       Se encarga de inicializar todo lo que necesita, para después ejecutarse en
       un bucle hasta que la función retorna."""

# Iniciliza todo
    pygame.init()

    ## Establece las dimensiones de la pantalla e importa los elementos para el
    ## fondo o background si se desea personalizar (lineas 139 a 142), un fondo
    ## sólido puede aplicarse en su defecto
    screen = pygame.display.set_mode((800, 800))
    background = pygame.image.load("/home/hectorsalamanca/juego/back.png").convert()
    background = pygame.transform.scale(background, (800, 800))
    screen.blit(background, (0, 0))
    pygame.display.flip()

    ## Establece el título de la ventana de la aplicación cuando esta sea generada
    pygame.display.set_caption('Whac-A-MARIO')
    pygame.mouse.set_visible(0) # El mouse no será visible sobre la pantalla

    ## Modifica el ícono que acompaña al título en la ventana de la aplicación, de
    ## no hacerlo, colocará el ícono por defecto de la ventana de pygame
    icono = pygame.image.load("/home/hectorsalamanca/juego/icon.jpg").convert()
    pygame.display.set_icon(icono)


    ## Colóca un texto en el fondo centrado
    if pygame.font:
        font = pygame.font.Font(None, 35) # Tamaño de la fuente
        text = font.render("ERES UN THWOMP, TU MISIÓN: ¡Aplastarlo!", 1, (10, 10, 10))
        textpos = text.get_rect(centerx=int(background.get_width()/2)) # Posición del texto
        background.blit(text, textpos)

    ## Permite visualizar el fondo establecido lineas arriba
    screen.blit(background, (0, 0))
    pygame.display.flip()

    ## Prepara los objetos necesarios para hacer funcionar al juego (funciones y
    ## elementos complementarios)
    clock = pygame.time.Clock()
    #whiff_sound = pygame.mixer.Sound("/home/hectorsalamanca/juego/Thwomp.wav") # Importa un elemento de sonido .wav
    
    #punch_sound = pygame.mixer.Sound("/home/hectorsalamanca/juego/damage.wav") # Importa un elemento de sonido .wav0
    
    pygame.mixer.init()
    th = "/home/hectorsalamanca/juego/Thwomp.wav"
    ma = "/home/hectorsalamanca/juego/damage.wav"
    
    
    punch_sound = pygame.mixer.music.load(ma)
    
    

    mario = Mario() # Constructores para la clase Mario
    thwomp = Thwomp() # Constructores para la clase Thwomp
    allsprites = pygame.sprite.RenderPlain((thwomp, mario)) # Crea los sprites


   ## Loop Principal (Main Loop)
    going = True
    while going:
        clock.tick(60)

        # Gestión de eventos de entrada
        for event in pygame.event.get():
            if event.type == QUIT:
                going = False
            elif event.type == KEYDOWN and event.key == K_ESCAPE:
                going = False
            elif event.type == MOUSEBUTTONDOWN:
                if thwomp.punch(mario):
                    pygame.mixer.music.load(ma)
                    pygame.mixer.music.play() # Reproduce el sonido para el golpe en Mario
                    mario.punched()
                else:
                    pygame.mixer.music.load(th)
                    pygame.mixer.music.play() # Reproduce el sonido para el Thwomp (fallar)
            elif event.type == MOUSEBUTTONUP:
                thwomp.unpunch()

        allsprites.update()

        # Dibuja todos los elementos
        screen.blit(background, (0, 0))
        allsprites.draw(screen)
        pygame.display.flip()

    pygame.quit()

## Se cierra la ventana y, por tanto, el juego


# Esto llama a la función 'main' cuando se ejecuta este script
if __name__ == '__main__':
    main()
