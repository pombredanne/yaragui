cmake_minimum_required(VERSION 3.0)

# where is yara?
set(YaraPath ./yara-3.4.0)
include_directories(${YaraPath}/libyara/include)
link_directories(${YaraPath}/libyara/.libs)

# where is boost?
include_directories(/opt/boost)
link_directories(/opt/boost/stage/lib)

# where is Qt5?
set(CMAKE_PREFIX_PATH "/data/util/qt5/5.3/gcc_64")

set(AUTOUIC TRUE)
set(AUTOMOC TRUE)
set(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/bin/qmake)
find_package(Qt5Widgets)

include_directories(.)

set(Sources
  src/main.cpp
  src/asio_events.cpp
  src/main_controller.cpp
  src/scanner.cpp
  src/scanner_rule.cpp
  src/main_window.cpp
)

QT5_WRAP_CPP(Sources
  src/asio_events.h
  src/main_window.h
)

QT5_WRAP_UI(Sources
  src/ui/main_window.ui
)

set(Libraries
  ${Qt5Widgets_LIBRARIES}
  boost_system
  boost_thread
  yara
)

add_executable(yaragui ${Sources})
target_link_libraries(yaragui ${Libraries})
