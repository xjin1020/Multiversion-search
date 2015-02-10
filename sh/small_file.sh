#!/bin/bash
find . -type f | xargs -I'{}' sh -c 'echo {} `stat -c %s {}`'
